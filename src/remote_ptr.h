/* -*- Mode: C++; tab-width: 8; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#ifndef RR_REMOTE_PTR_H_
#define RR_REMOTE_PTR_H_

#include <cstddef>

/**
 * A pointer in some tracee address space.
 * This lets us distinguish between real, usable pointers in rr's address space
 * and pointers that only make sense in a tracee address space.
 */
template <typename T> class remote_ptr {
public:
  remote_ptr() : ptr(0) {}
  remote_ptr(uintptr_t ptr) : ptr(ptr) {}
  remote_ptr(std::nullptr_t null) : ptr(0) {}
  remote_ptr(T* ptr) : ptr(reinterpret_cast<uintptr_t>(ptr)) {}
  template <typename U> remote_ptr(remote_ptr<U> p) : ptr(p.as_int()) {
    consume_dummy(static_cast<U*>(nullptr));
  }

  operator T*() const { return reinterpret_cast<T*>(ptr); }
  uintptr_t as_int() const { return ptr; }

  template <typename U> remote_ptr<U> cast() { return remote_ptr<U>(ptr); }

  bool operator<(const remote_ptr<T>& other) const { return ptr < other.ptr; }
  bool operator==(const remote_ptr<T>& other) const { return ptr == other.ptr; }

private:
  void consume_dummy(T*) {}

  uintptr_t ptr;
};

inline remote_ptr<void> operator+(const remote_ptr<void>& p, intptr_t bytes) {
  return remote_ptr<void>(p.as_int() + bytes);
}

inline remote_ptr<void>& operator+=(remote_ptr<void>& p, intptr_t bytes) {
  p = p + bytes;
  return p;
}

inline remote_ptr<void> operator-(const remote_ptr<void>& p, intptr_t bytes) {
  return remote_ptr<void>(p.as_int() - bytes);
}

inline intptr_t operator-(const remote_ptr<void>& p1,
                          const remote_ptr<void>& p2) {
  return p1.as_int() - p2.as_int();
}

#endif /* RR_REMOTE_PTR_H_ */