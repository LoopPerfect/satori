#ifndef SATORI_MANAGED_PTR
#define SATORI_MANAGED_PTR

namespace satori {

/**
 *  managed_ptr
 *
 *  A pointer to a resource that is already being managed for you.
 *  We wrap the raw in a managed_ptr to convey the ownership semantics.
 */
template <typename T>
struct managed_ptr {

private:
  T* pointer;

public:
  managed_ptr(T* pointer) : pointer(pointer) {}
  managed_ptr(managed_ptr<T> const& ptr) : pointer(ptr.pointer) {}

  ~managed_ptr() {}

  T* get() { return pointer; }
  T const* get() const { return pointer; }

  T& operator*() { return *pointer; }
  T const& operator*() const { return *pointer; }

  T* operator->() { return pointer; }
  T const* operator->() const { return pointer; }

  operator bool() const { return pointer != nullptr; }
};

template <typename T>
bool operator==(managed_ptr<T> const& lhs, managed_ptr<T> const& rhs) {
  return lhs.get() == rhs.get();
}

template <typename T>
bool operator!=(managed_ptr<T> const& lhs, managed_ptr<T> const& rhs) {
  return !(lhs == rhs);
}

} // namespace satori

#endif
