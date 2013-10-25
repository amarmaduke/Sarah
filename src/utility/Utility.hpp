
#ifndef SARAH_UTILITY_HPP
#define SARAH_UTILITY_HPP

#include <typeinfo>
#include <typeindex>

namespace sarah {

// -------------------------------------------------------------------------- //
// Facilities

/// Attempts to dynamically convert u to T.
template<typename T, typename U>
  inline T* 
  as(U* u) { return dynamic_cast<T*>(u); }

template<typename T, typename U>
  inline const T* 
  as(const U* u) { return dynamic_cast<const T*>(u); }

/// Attempts to dynamically convert u to T. Note that this overload will
/// throw an exception if the conversion is not possible.
template<typename T, typename U>
  inline T& 
  as(U& u) { return dynamic_cast<T&>(u); }

template<typename T, typename U>
  inline const T& 
  as(const U& u) { return dynamic_cast<const T&>(u); }


/// Returns true only if u can be dynamically converted to T.
template<typename T, typename U>
  inline bool
  is(const U* u) { return as<T>(u); }

/// Returns true only if u can be dynamically converted to T.
template<typename T, typename U>
  inline bool
  is(const U& t) { return is<T>(&t); }


/// Returns type information for the type of t.
template<typename T>
  inline std::type_index
  kind(const T* t) { return typeid(*t); }

template<typename T>
  inline std::type_index
  kind(const T& t) { return kind(&t); }


} // namespace sarah

#endif
