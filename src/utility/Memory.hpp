
#ifndef SARAH_MEMORY_HPP
#define SARAH_MEMORY_HPP

#include <memory>
#include <list>
#include <set>

namespace sarah {


/// A basic factory is responsible for the allocation and management of
/// objects of the specified type.
template<typename T>
  struct Basic_factory : std::list<T>
  {
    template<typename... Args>
      T& make(Args&&... args) {
        this->emplace_back(std::forward<Args>(args)...);
        return this->back();
      }
  };

/// A singleton factory contains a single object of the specified type.
/// It is allocated on the first request and every subsequent request
/// returns the same value.
///
/// Repeated allocation of the same object with different arguments
/// is undefined.
template<typename T>
  struct Singleton_factory
  {
    Singleton_factory()
      : value(nullptr) { }
    
    ~Singleton_factory() {
      if (value)
        delete value;
    }

    template<typename... Args>
      T& make(Args&&... args) {
        if (not value)
          value = new T(std::forward<Args>(args)...);
        return *value;
      }

    T* value;
  };

} // namespace sarah

#endif
