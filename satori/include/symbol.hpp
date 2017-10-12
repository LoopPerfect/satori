#ifndef SATORI_SYMBOL_HPP
#define SATORI_SYMBOL_HPP

#include <memory>
#include <satori/lib.hpp>

namespace satori {

  template <typename T>
  struct Symbol {

  private:

    // We need to keep the Lib alive since it owns pointer
    std::shared_ptr<Lib> lib;
    T* pointer;

  public:

    Symbol(std::shared_ptr<Lib> lib, T* pointer) : lib(lib), pointer(pointer) {}

    T* get() const {
      return pointer;
    }

    T* operator -> () { return pointer; }

    T const* operator -> () const { return pointer; }

  };

}

#endif
