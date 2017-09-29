#ifndef SATORI_ROUTER_HPP
#define SATORI_ROUTER_HPP

#include <functional>
#include <vector>
#include <deque>
#include <string>
#include <neither/maybe.hpp>

#include <r3.hpp>

namespace satori {

  using Params = std::vector<std::string>;

  template<class T>
  struct Match {
    std::string path;
    Params params;
    T data;
  };

  template<class T>
  struct RouteData {
    std::string route;
    T data;
  };

  template<class T=std::function<void(std::string, std::vector<std::string>)>>
  struct Router {

    using Data = RouteData<T>;

    r3::Tree tree;
    std::deque<Data> callbacks;

    bool hasCompiled = false;

    Router() : tree(10) {}

    neither::Maybe<std::string> addRoute(std::string const& route, T const& data) {
      assert(!hasCompiled && "You cannot add routes after calling compile");
      callbacks.push_back({ route, data });
      auto* dataAddress = &callbacks.back();
      char* errorString;
      r3::Node node = tree.insert_pathl(
        dataAddress->route.c_str(),
        dataAddress->route.size(),
        (void*)dataAddress,
        &errorString);
      // Failure?
      if (node == nullptr) {
        // Remove the callback
        callbacks.pop_back();
        // Copy the error message into an std::string and delete the c-string.
        // This is a bit slower, but it prevents memory leaks.
        auto error = std::string(errorString);
        delete [] errorString;
        return neither::Maybe<std::string>(error);
      }
      return neither::Maybe<std::string>();
    }

    int compile() {
      assert(!hasCompiled && "compile must only be called once");
      hasCompiled = true;
      return tree.compile(nullptr);
    }

    neither::Maybe<Match<T>> match(std::string const& path) {
      assert(hasCompiled && "match must be called after compile");

      auto matchedNode = tree.matchl(path.c_str(), path.size());

      r3::MatchEntry entry(path.c_str());
      matchedNode = tree.match_entry(entry);

      if (!matchedNode) {
        return neither::Maybe<Match<T>>();
      }

      Params params;
      match_entry* e = entry.get();
      for (int i = 0;  i < e->vars.tokens.size; ++i) {
        auto const value = e->vars.tokens.entries[i];
        params.push_back({value.base, value.len});
      }

      auto* routeData = static_cast<Data*>(matchedNode.data());

      auto match = Match<T>{ path, params, routeData->data };

      return {
        std::move(match)
      };
    }
  };

}

#endif
