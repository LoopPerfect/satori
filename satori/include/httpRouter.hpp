#ifndef SATORI_HTTP_ROUTER_HPP
#define SATORI_HTTP_ROUTER_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <satori/httpParser.hpp>
#include <satori/httpRequest.hpp>
#include <satori/httpResponse.hpp>
#include <satori/loop.hpp>
#include <satori/router.hpp>

namespace satori {

constexpr unsigned convertToR3(unsigned const method) {
  switch (method) {
  case HTTP_DELETE:
    return METHOD_DELETE;
  case HTTP_PUT:
    return METHOD_PUT;
  case HTTP_GET:
    return METHOD_GET;
  case HTTP_POST:
    return METHOD_POST;
  case HTTP_PATCH:
    return METHOD_PATCH;
  case HTTP_HEAD:
    return METHOD_HEAD;
  case HTTP_OPTIONS:
    return METHOD_OPTIONS;
  default:
    return ~0;
  }
}

template <class X, class Y> constexpr void chain(X &&, Y &&) {}

template <class X, class Y, class F>
constexpr void chain(X &&req, Y &&res, F &&f) {
  f(std::forward<X>(req), std::forward<Y>(res));
}

template <class X, class Y, class F, class... Fs>
constexpr void chain(X &&req, Y &&res, F &&f, Fs const &... fs) {
  f(std::forward<X>(req), std::forward<Y>(res),
    [fs...](auto &&req, auto &&res) {
      chain(std::forward<decltype(req)>(req), std::forward<decltype(res)>(res),
            fs...);
    });
}

template <class Req, class Res> struct Route {
  std::string route;
  int method;
  std::function<void(Req, Res)> handler;

  Route(std::string const &route, int const method,
        std::function<void(Req, Res)> const &handler)
      : route{route}, method{method}, handler{handler} {}
};

template <class Req = HttpRequest, class Res = HttpResponse> struct AppRouter {
  std::vector<Route<Req, Res>> routes;

  template <class F, class... Fs>
  AppRouter &get(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_GET, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &post(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_POST, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &put(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_PUT, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &delet(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_DELETE, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &patch(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_PATCH, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &head(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_HEAD, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &options(std::string const &path, F f, Fs... fs) {
    return matching(METHOD_OPTIONS, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &any(std::string const &path, F f, Fs... fs) {
    return matching(~0, path, f, fs...);
  }

  template <class F, class... Fs>
  AppRouter &matching(int methods, std::string const &path, F f, Fs... fs) {
    routes.emplace_back(Route<Req, Res>{
        path, methods, [=](auto x, auto y) { chain(x, y, f, fs...); }});

    return *this;
  }

  void apply(Loop *loop, Tcp *server) {

    auto router = std::make_shared<satori::Router<Route<Req, Res>>>();

    for (auto const &r : routes) {
      router->addRoute(r.method, r.route, r);
    }

    router->compile();

    server->onListen = [=](int status) {
      if (status < 0)
        return; // flaky client

      auto client = loop->newTcp();
      server->accept(client);

      client->read();

      // TODO: make parser part of the connection/request state...
      auto parser = std::make_shared<satori::HttpParser>(HTTP_REQUEST);

      client->onData = [parser](int status, StringView buffer) {
        parser->execute(buffer);
      };

      auto key = std::make_shared<std::string>();
      auto url = std::make_shared<std::string>();
      auto headers = std::make_shared<std::map<std::string, std::string>>();

      parser->onHeaderField = [=](http_parser const *parser, char const *at,
                                  size_t len) {
        *key = std::string(at, len);
        return 0;
      };

      parser->onHeaderValue = [=](http_parser const *parser, char const *at,
                                  size_t len) {

        // TODO: avoid pointer indirection
        (*headers)[*key] = std::string(at, len);
        return 0;
      };

      parser->onHeadersComplete = [=](auto p) {
        auto method = convertToR3(p->method);
        if (auto match = router->match(method, *url)) {
          auto const &data = match.value.data;
          auto const &handler = data.handler;

          handler({*url, http_method_str((http_method)p->method), *headers,
                   match.value.params},
                  {loop, client});

        } else {
          loop->newWrite(client, "HTTP/1.0 404 NOT FOUND\r\n")->onWriteEnd =
              [=](auto) { client->close(); };
        }
        return 0;
      };

      parser->onUrl = [=](http_parser const *parser, char const *at,
                          size_t len) {
        *url = std::string(at, len);
        return 0;
      };
    };
  }
};

} // namespace satori

#endif
