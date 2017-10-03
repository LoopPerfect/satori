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

struct Connection {
  std::string url;
  std::string method;
  std::map<std::string, std::string> headers;
  std::vector<std::string> params;
  Loop* loop;
  Tcp* client;
};


struct Response {

  Loop* loop;
  Tcp* client;

  std::map<std::string, std::string> headers;
  int code;
  std::string buffer;


  Response& set(std::map<std::string, std::string> const& headers) {
    for (auto const& e: headers) {
      headers[e.first] = e.second;
    }
    return *this;
  }


  Response& status(unsigned short const& code) {
    this->code = code;
    return *this;

  }
  
  Response& write(std::string const& str) {

  }

  Response& end(std::string const& str) {

  }


};

template<class X>
void chain(X) {}

template<class X, class F>
void chain(X const& x, F const& f) {
  f(x);
}

template<class X, class F, class...Fs>
void chain(X const& x, F const& f, Fs const&...fs) {
  f(x, [=](auto y) -> decltype(chain(y, fs...)) {
    chain(y, fs...);
  });
}

template<class X>
struct Route {
  std::string route;
  std::string method;
  std::function<void(X)> handler;

  Route(
      std::string const& route, 
      std::string const& method,  
      std::function<void(X)> const& handler)
    : route{route}
    , method{method}
    , handler{handler}
  {}
};


template<class X = Connection>
struct AppRouter {
  std::vector<Route<X>> routes;
  
  template<class F, class...Fs>
  AppRouter& get(std::string const& path, F f, Fs...fs) {
    routes.emplace_back(Route<X>  {path, "GET", [=](auto x) {
      chain(x, f, fs...);
    }});

    return *this;
  }



  void apply(Loop* loop, Tcp* server) {

    auto router = make_shared<satori::Router<Route<X>>>();

    for (auto r : routes) {
      router->addRoute(r.route, r);
    }

    router->compile();


    server->onListen = [=](auto status) {
      auto client = loop->newTcp();
      server->accept(client);

      client->read();

      auto parser = std::make_shared<satori::HttpParser>(HTTP_REQUEST);
      
      client->onData = [parser](const char* str, size_t len) {
        parser->execute(std::string(str,len));
      };

    
      auto key = std::make_shared<std::string>(); 
      auto url = std::make_shared<std::string>();
      auto headers = std::make_shared<
        std::map<std::string, std::string>>(); 

      parser->onHeaderField = [=](
        http_parser const* parser, 
        char const* at, 
        size_t len) {
        //std::cout << "key: " <<*key << std::endl;
        *key = std::string(at, len);
        return 0;
      };

      parser->onHeaderValue = [=](
          http_parser const* parser, 
          char const* at, 
          size_t len) {

        //std::cout << "header: "<<  *key <<"="<<  std::string(at, len) << std::endl;
        (*headers)[*key] = std::string(at, len);
        return 0;
      };

      parser->onHeadersComplete = [=](auto) {
        Connection con;
        con.client = client;
        con.url = *url;
        con.headers = *headers;
        if(auto match = router->match(con.url)) {
          auto data = match.value.data;
          auto handler = data.handler;
          con.loop = loop;
          con.params = match.value.params;
          handler(con);
        } else {
          loop->newWrite(con.client, "HTTP/1.0 404 NOT FOUND\r\n")
            ->onWriteEnd = [=](auto){
            con.client->close();
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

  app.get("/", 
      //parseHeaders("contentType", "accept"), 
      [=](auto con) {
    loop->newWrite(con.client, "HTTP/1.0 200 OK\r\n\r\n");
      
    std::string res = "headers:\n";
    for(auto h: con.headers) {
      res += h.first + ": " + h.second + "\n";
    }

    loop->newWrite(con.client, res)
      ->onWriteEnd = [con](auto) {
      con.client->close();
    };
  });


  app.get("/{id}", [=](auto con) {
    loop->newWrite(con.client, "HTTP/1.0 200 OK\r\n\r\n");
      
    std::string res = "headers:\n";
    for(auto h: con.headers) {
      res += h.first + ": " + h.second + "\n";
    }

    res += "params: ";
    for(auto p: con.params) {
      res += p;
    }

    loop->newWrite(con.client, res)
      ->onWriteEnd = [con](auto) {
      con.client->close();
    };
  });

  auto server = loop->newTcp();
  server->listen("127.0.0.1", 8080);
  app.apply(loop.get(), server);

  loop->run();
}
