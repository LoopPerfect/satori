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

  struct Match {
    std::string path;
    Params params;
  };

  template<class T>
  struct RouteData {
    std::string route;
    T callback;
  };


  template<class T = std::function<void(Match)> >
  struct Router {

    using Data = RouteData<T>;
    r3::Tree tree;
    std::deque<Data> callbacks;

    Router() : tree(10) {}

    neither::Maybe<std::string> addRoute(std::string const& route, T const& cb) {
      callbacks.push_back({route, cb});
      Data* data = &callbacks.back();
      char* errorString;
      r3::Node node = tree.insert_pathl(
        data->route.c_str(),
        data->route.size(),
        (void*)data,
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
      return tree.compile(nullptr);
    }

    std::function<void()> match(std::string const& path) {
      auto matchedNode = tree.matchl(path.c_str(), path.size());
 
      r3::MatchEntry entry(path.c_str());
      matchedNode = tree.match_entry(entry);

      if (!matchedNode) {
        return {}; 
      }

        
      Params params;
      match_entry* e = entry.get();
      for(int i=0;  i < e->vars.tokens.size; ++i) { 
        auto const value = e->vars.tokens.entries[i];
        params.push_back({value.base, value.len});
      }

      auto match = Match{path, params};
      Data* data = static_cast<Data*>(matchedNode.data());

      return [match = std::move(match), data] {
        data->callback(match);
      };
    }
  };

}

#endif
