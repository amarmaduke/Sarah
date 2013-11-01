
#ifndef SARAH_STRUCTURE_HPP
#define SARAH_STRUCTURE_HPP

#include <tuple>

namespace sarah {

// Atomic structures in the language.
template<typename T>
  struct Atom {
    Atom(const T& x)
      : data(x) { }

    const T& first() const { return data; }

    T data;
  };

// Non-atomic structures in the language.
template<typename... Args>
  struct Structure;

template<typename T>
  struct Structure<T> : std::tuple<const T*> {
    Structure(const T& t)
      : std::tuple<const T*>(&t) { }

    const T& first() const { return *std::get<0>(*this); }
  };

template<typename T1, typename T2>
  struct Structure<T1, T2> : std::tuple<const T1*, const T2*> {
    Structure(const T1& t1, const T2& t2)
      : std::tuple<const T1*, const T2*>(&t1, &t2) { }

    const T1& first() const { return *std::get<0>(*this); }
    const T2& second() const { return *std::get<1>(*this); }
  };

template<typename T1, typename T2, typename T3>
  struct Structure<T1, T2, T3> : std::tuple<const T1*, const T2*, const T3*> {
    Structure(const T1& t1, const T2& t2, const T3& t3)
      : std::tuple<const T1*, const T2*, const T3*>(&t1, &t2, &t3) { }

    const T1& first() const { return *std::get<0>(*this); }
    const T2& second() const { return *std::get<1>(*this); }
    const T3& third() const { return *std::get<2>(*this); }
  };


} // namespace sarah

#endif
