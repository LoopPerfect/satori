#ifndef SATORI_DYNAMIC_LIBRARY_HPP
#define SATORI_DYNAMIC_LIBRARY_HPP

#include <memory>
#include <string>
#include <neither/neither.hpp>
#include <satori/lib.hpp>
#include <satori/symbol.hpp>

namespace satori {

  struct DynamicLibrary {

  private:

    std::shared_ptr<Lib> state;

  public:

    DynamicLibrary() : state(std::make_shared<Lib>()) {

    }

    neither::Maybe<std::string> open(std::string const& filename) {
      int result = state->open(filename);
      if (result == 0) {
        return neither::Maybe<std::string>();
      }
      return neither::Maybe<std::string>(state->dlerror());
    }

    neither::Either<std::string, Symbol> find(std::string const& symbolName) {
      void* data = nullptr;
      int result = state->dlsyn(symbolName, &data);
      if (result == 0) {
        auto symbol = Symbol(state, data);
        return neither::Either<std::string, Symbol>::rightOf(symbol);
      }
      return neither::Either<std::string, Symbol>::leftOf(state->dlerror());
    }

  };

}

#endif
