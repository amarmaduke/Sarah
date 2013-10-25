
#include <stdexcept>

#include "Integer.hpp"

namespace sarah {

// NOTE: GMP does not optimize for default-intialized values. That is
// mpz_init allocates memory, which will result in an inefficient move
// implementation. 

Integer::Integer() { mpz_init(value); }

Integer::Integer(const Integer& x) {
  mpz_init_set(value, x.value); 
}

Integer&
Integer::operator=(const Integer& x) {
  if (this != &x) {
    mpz_clear(value);
    mpz_set(value, x.value);
  }
  return *this;
}

// Construct an integer with the value n.
Integer::Integer(long n) { mpz_init_set_si(value, n); }

// Consruct an integer with the value in s in base b. Behavior is undefined
// if s does not represent an integer in base b.
Integer::Integer(String s, int b) { 
  if (mpz_init_set_str(value, s.data(), b) == -1)
    throw std::runtime_error("invalid integer representation");
}

Integer::~Integer() { mpz_clear(value); }


Integer& 
Integer::operator+=(const Integer& x) {
  mpz_add(value, value, x.value);
  return *this;
}

Integer& 
Integer::operator-=(const Integer& x) {
  mpz_sub(value, value, x.value);
  return *this;
}

Integer& 
Integer::operator*=(const Integer& x) {
  mpz_mul(value, value, x.value);
  return *this;
}

// Divide this integer value by x. Integer division is implemented as
// floor division. A discussion of alternatives can be found in the paper,
// "The Euclidean definition of the functions div and mod" by Raymond T.
// Boute (http://dl.acm.org/citation.cfm?id=128862).
Integer& 
Integer::operator/=(const Integer& x) {
  mpz_fdiv_q(value, value, x.value);
  return *this;
}

// Compute the remainder of the division of this value by x. Integer division
// is implemented as floor division. See the notes on operator/= for more
// discussion.
Integer& 
Integer::operator%=(const Integer& x) {
  mpz_fdiv_r(value, value, x.value);
  return *this;
}

// Returns true when the two integers have the same value.
bool
operator==(const Integer& a, const Integer& b) {
  return mpz_cmp(a.data(), b.data()) == 0;
}

// Returns true when a is less than b.
bool
operator<(const Integer& a, const Integer& b) {
  return mpz_cmp(a.data(), b.data()) < 0;
}

// Returns the number of bits in the integer representation.
std::size_t
Integer::bits() const { return mpz_sizeinbase(value, 2); }


} // namespace sarah
