
#ifndef SARAH_STRING_H
#define SARAH_STRING_H

#include <string>
#include <iosfwd>

#include "Utility.hpp"
#include "Ios.hpp"

namespace sarah {

// The String class is a handle to an interned string. Its usage guarantees
// that each unique occurrence of a string in the text of a program appears
// only once in the memory of the program.
//
// The String class is a regular, but reference semantic type.
class String {
public:
  using iterator       = std::string::const_iterator;
  using const_iterator = std::string::const_iterator;

  String() 
    : str_(nullptr) 
  { }
  
  String(const std::string& s) 
    : str_(intern(s)) 
  { }

  String(const char* s)
    : String(std::string(s))
  { }

  String(const char* s, std::size_t n)
    : String(std::string(s, n))
  { }

  String(const char* f, const char* l)
    : String(f, l - f)
  { }


  // Returns the underlying string pointer.
  const std::string* ptr() const { return str_; }

  // Returns a reference to the underlying string.
  const std::string& str() const { return *str_; }

  /// Returns a pointer to the underlying character data.
  const char* data() const { return str().c_str(); }

  // Iterators
  iterator       begin()       { return str_->begin(); }
  const_iterator begin() const { return str_->begin(); }

  iterator       end()       { return str_->end(); }
  const_iterator end() const { return str_->end(); }

private:
  static const std::string* intern(const std::string&);

private:
  const std::string* str_;
};

// Returns true when two strings refer to the same object.
inline bool
operator==(String a, String b) { return a.ptr() == b.ptr(); }

inline bool
operator!=(String a, String b) { return a.ptr() != b.ptr(); }

// Returns true when a points to an object with a lesser address
// than b. This operation does not define a lexicographical order.
inline bool
operator<(String a, String b) { return a.ptr() < b.ptr(); }

inline bool
operator>(String a, String b) { return a.ptr() > b.ptr(); }

inline bool
operator<=(String a, String b) { return a.ptr() <= b.ptr(); }

inline bool
operator>=(String a, String b) { return a.ptr() >= b.ptr(); }

// Streaming
template<typename C, typename T>
  inline std::basic_ostream<C, T>&
  operator<<(std::basic_ostream<C, T>& os, String s) { return os << s.str(); }

} // namespace sarah


namespace std {

// Hash support for Strings.
template<>
struct hash<sarah::String> {
  std::size_t 
  operator()(sarah::String str) const {
    hash<const void*> h;
    return h(str.ptr());
  }
};

} // namespace std

#endif
