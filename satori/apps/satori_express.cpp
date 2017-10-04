#include <assert.h>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <tuple>
#include <map>
#include <type_traits>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <satori/satori.hpp>
#include <satori/router.hpp>
#include <satori/status.hpp>

using namespace std;
using namespace satori;

struct HttpRequest {
  std::string url;
  std::string method;
  std::map<std::string, std::string> headers;
  std::vector<std::string> params;
};


struct ResponseBuffer {
  Loop* loop;
  Tcp* client;
  std::string buffer = "";

  ResponseBuffer(Loop* loop, Tcp* client)
    : loop{loop}
    , client{client}
  {}

  ResponseBuffer& protocol(std::string const& proto = "HTTP/1.0") {
    return write(proto+" ");
  }

  ResponseBuffer& status(unsigned short const code) {
    return write(code2Msg(code));
  }

  ResponseBuffer& set(std::map<std::string, std::string> const& headers) {
    for (auto const& e : headers) {
      write(e.first+": "+ e.second+"\r\n");
    }
    return write("\r\n");
  }
  
  ResponseBuffer& write(std::string const& str) {
    buffer += str; 
    return *this;
  }

  Write* flush() {
    auto w = loop->newWrite(client, buffer);
    buffer = "";
    return w;
  }

  void end(std::string const& str = "") {
    write(str)
      .flush()->onWriteEnd = [client = this->client](auto) {
        client->close();   
    };
    client = nullptr;
  }

  ~ResponseBuffer() {
    if (client) end();
  }

};


struct HttpResponse {
  std::shared_ptr<ResponseBuffer> buffer;

  std::map<std::string, std::string> headers = {};
  unsigned short statusCode = 200;
  bool headersSent = false;
  std::string protocol = "HTTP/1.0";

  HttpResponse(Loop* loop, Tcp* tcp)
    : buffer(std::make_shared<ResponseBuffer>(loop, tcp))
  {}

  HttpResponse(HttpResponse const& r) 
    : buffer(r.buffer)
  {}

  HttpResponse(HttpResponse && r) 
    : buffer(std::move(r.buffer))
  {}

  HttpResponse() = delete;
  ~HttpResponse(){}

  HttpResponse http(std::string const& str) {
    protocol = "HTTP/"+str;
    return *this;
  }

  HttpResponse& status(unsigned short const status) {
    statusCode = status;
    return *this;
  }

  HttpResponse& set(std::string const& key, std::string const& value) {
    headers[key] = value;
    return *this;
  }

  HttpResponse& set(std::map<std::string, std::string> const& hdrs) {
    for (auto const& e : hdrs) {
      set(e.first, e.second);
    }
    return *this;
  }

  HttpResponse& write(std::string const& str) {
    assert(buffer && "write after close");
    if (!headersSent) {
      buffer
        ->protocol(protocol)
        .status(statusCode)
        .set(headers);
      headersSent = true;
    }

    buffer->write(str);
    return flush();
  }

  HttpResponse& flush() {
    buffer->flush();
    return *this;
  }

  HttpResponse& end(std::string const& str="") {
    buffer->end(str);
    return *this;
  }

  void close() {
    buffer.reset();
  }

};




template<class X, class Y>
constexpr void chain(X&&, Y&&) {}

template<class X, class Y, class F>
constexpr void chain(X&& req, Y&& res, F&& f) {
  f( std::forward<X>(req), std::forward<Y>(res) );
}

template<class X, class Y, class F, class...Fs>
constexpr void chain(X&& req, Y&& res, F&& f, Fs const&...fs) {
  f(std::forward<X>(req), 
    std::forward<Y>(res), 
    [fs...](auto&& req, auto&& res) {
      chain(
        std::forward<decltype(req)>(req), 
        std::forward<decltype(res)>(res), 
        fs...);
  });
}

template<class Req, class Res>
struct Route {
  std::string route;
  std::string method;
  std::function<void(Req, Res)> handler;

  Route(
      std::string const& route, 
      std::string const& method,  
      std::function<void(Req, Res)> const& handler)
    : route{route}
    , method{method}
    , handler{handler}
  {}
};


template<class Req = HttpRequest, class Res = HttpResponse>
struct AppRouter {
  std::vector<Route<Req, Res>> routes;
  
  template<class F, class...Fs>
  AppRouter& get(std::string const& path, F f, Fs...fs) {
    routes.emplace_back(Route<Req, Res>  {path, "GET", [=](auto x, auto y) {
      chain(x, y, f, fs...);
    }});

    return *this;
  }

  void apply(Loop* loop, Tcp* server) {

    auto router = make_shared<satori::Router<Route<Req, Res>>>();

    for (auto const& r : routes) {
      router->addRoute(r.route, r);
    }

    router->compile();


    server->onListen = [=](int status) {
      if (status<0) return; // flaky client

      auto client = loop->newTcp();
      server->accept(client);

      client->read();

      // TODO: make parser part of the connection/request state...
      auto parser = std::make_shared<satori::HttpParser>(HTTP_REQUEST);
      
      client->onData = [parser](const char* str, size_t len) {
        parser->execute(std::string(str, len));
      };

    
      auto key = std::make_shared<std::string>(); 
      auto url = std::make_shared<std::string>();
      auto headers = std::make_shared<
        std::map<std::string, std::string>>(); 

      parser->onHeaderField = [=](
        http_parser const* parser, 
        char const* at, 
        size_t len) {
        *key = std::string(at, len);
        return 0;
      };

      parser->onHeaderValue = [=](
          http_parser const* parser, 
          char const* at, 
          size_t len) {

        //TODO: avoid pointer indirection
        (*headers)[*key] = std::string(at, len);
        return 0;
      };

      parser->onHeadersComplete = [=](auto) {
        if (auto match = router->match(*url)) {
          auto const& data = match.value.data;
          auto const& handler = data.handler;

          handler({
              *url,
              "GET",
              *headers,
              match.value.params
          }, {
              loop,
              client
          });


        } else {
          loop->newWrite(client, "HTTP/1.0 404 NOT FOUND\r\n")
            ->onWriteEnd = [=](auto){
            client->close();
          };
        }
        return 0;
      };


      parser->onUrl = [=](
          http_parser const* parser, 
          char const* at, 
          size_t len) {
          *url = std::string(at, len);
        return 0;
      };
    };
  }

};


int main() {

  auto loop = std::make_shared<Loop>();

  AppRouter<> app;

  app.get("/", [] (auto req, auto res) { 
    res.status(200)
      .write("hello world!");
  });


  app.get("/{id}", [](auto req, auto res) {
    
    std::string response = "headers:\n";
    for(auto const& h: req.headers) {
      response += h.first + ": " + h.second + "\n";
    }

    response += "params: ";
    for (auto const& p: req.params) {
      response += p;
    }

    res.status(200)
      .write(response);
  });



  auto server = loop->newTcp();
  server->listen("127.0.0.1", 8080);
  app.apply(loop.get(), server);

  loop->run();
}
