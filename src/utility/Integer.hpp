
#ifndef SARAH_INTEGER_HPP
#define SARAH_INTEGER_HPP

#include <gmp.h>

#include "String.hpp"
#include "Memory.hpp"

namespace sarah {

// The Integer class represents arbitrary integer values.
//
// \todo GMP is not a particularly efficient implementation.
class Integer {
public:
  // Default constructor
  Integer();

  // Copy semantics
  Integer(const Integer&);
  Integer& operator=(const Integer&);

  // Value initialization
  Integer(long);
  Integer(String, int = 10);

  // Destructor
  ~Integer();

  // Arithmetic compound assignmnt operators.
  Integer& operator+=(const Integer&);
  Integer& operator-=(const Integer&);
  Integer& operator*=(const Integer&);
  Integer& operator/=(const Integer&);
  Integer& operator%=(const Integer&);

  // Observers
  std::size_t bits() const;

  const mpz_t& data() const { return value; }
  
private:
  mpz_t value; // Current integer value
};

// Equality
bool operator==(const Integer& a, const Integer& b);

inline bool 
operator!=(const Integer& a, const Integer& b) {
  return not(a == b);
}

// Ordering
bool operator<(const Integer& a, const Integer& b);

inline bool
operator>(const Integer& a, const Integer& b) {
  return b < a;
}

inline bool
operator<=(const Integer& a, const Integer& b) {
  return not(b < a);
}

inline bool
operator>=(const Integer& a, const Integer& b) {
  return not(a < b);
}

// Arithmetic
inline Integer
operator+(const Integer& a, const Integer& b) {
  return Integer(a) += b;
}

inline Integer
operator-(const Integer& a, const Integer& b) {
  return Integer(a) -= b;
}

inline Integer
operator*(const Integer& a, const Integer& b) {
  return Integer(a) *= b;
}

inline Integer
operator/(const Integer& a, const Integer& b) {
  return Integer(a) /= b;
}

inline Integer
operator%(const Integer& a, const Integer& b) {
  return Integer(a) %= b;
}

// Streaming
template<typename C, typename T>
  inline std::basic_ostream<C, T>&
  operator<<(std::basic_ostream<C, T>& os, const Integer& z) {
    int  base = stream_base(os);
    std::size_t n = mpz_sizeinbase(z.data(), base) + 1;
    std::unique_ptr<char[]> buf(new char[n]);
    switch (base) {
      case 8:
        gmp_snprintf(buf.get(), n, "%Zo", z.data());
        break;
      case 10:
        gmp_snprintf(buf.get(), n, "%Zd", z.data());
        break;
      case 16:
        gmp_snprintf(buf.get(), n, "%Zx", z.data());
        break;
    }
    return os << buf.get(); 
  }

} // namespace steve

#endif
