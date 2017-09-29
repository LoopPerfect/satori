#ifndef SATORI_ROUTER_HPP
#define SATORI_ROUTER_HPP

#include <functional>
#include <tuple>
#include <deque>
#include <string>
#include <iostream>

#include <neither/maybe.hpp>
#include <r3.hpp>

namespace satori {

  struct Router {

    r3::Tree tree;
    std::deque<std::function<void()>> callbacks;

    Router() : tree(10) {}

    neither::Maybe<std::string> addRoute(std::string const& route, std::function<void()> callback) {
      callbacks.push_back(callback);
      void* data = &callbacks.back();
      char* errorString;
      r3::Node node = tree.insert_pathl(
        route.c_str(),
        strlen(route.c_str()),
        data,
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

    neither::Maybe<std::tuple<int, std::string>> compile() {
      char* errorString;
      int errorCode = tree.compile(&errorString);
      // Success?
      if (errorCode == 0) {
        return neither::Maybe<std::tuple<int, std::string>>();
      }
      // Copy the error message into an std::string and delete the c-string.
      // This is a bit slower, but it prevents memory leaks.
      auto error = std::string(errorString);
      delete [] errorString;
      return neither::Maybe<std::tuple<int, std::string>>({ errorCode, error });
    }

    void match(std::string const& candidate) {
      auto matchedNode = tree.matchl(candidate.c_str(), strlen(candidate.c_str()));
      if (matchedNode) {
        int ret = *static_cast<int*>(matchedNode.data());
        std::cout << "match path ret: " << ret << std::endl;
      }

      r3::MatchEntry entry(candidate.c_str());
      matchedNode = tree.match_entry(entry);
      if (matchedNode) {
        int ret = *static_cast<int*>(matchedNode.data());
        std::cout << "match entry ret: " << ret << std::endl;
      }

    }
  };

}

#endif
