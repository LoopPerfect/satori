#ifndef SATORI_SYMBOL_HPP
#define SATORI_SYMBOL_HPP

#include <memory>
#include <satori/lib.hpp>

namespace satori {

  struct Symbol {

  private:

    // We need to keep the Lib alive since it owns ptr
    std::shared_ptr<Lib> lib;
    void* pointer;

  public:

    Symbol(std::shared_ptr<Lib> lib, void* pointer) : lib(lib), pointer(pointer) {}

    void* get() const {
      return pointer;
    }

    // T *operator->() { return (T*)pointer; }
    // T const *operator->() const { return (T*)pointer; }

  };

}

#endif
