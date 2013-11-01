
#ifndef STEVE_SEXPR_HPP
#define STEVE_SEXPR_HPP

#include <iosfwd>

// This module defines support for printing trees as sexprs.

namespace steve {

struct Token;
struct Tree;

template<typename> struct Tree_sequence;

// Sexpr-formatted output.
template<typename T>
  struct print_sexpr {
    print_sexpr(const T* t) : thing(t) { }
    const T* thing;
  };

template<typename T>
  struct print_sexpr<Tree_sequence<T>> {
    const Tree_sequence<T>& thing;
  };


// Allows node to be printed as an sexpr.
inline print_sexpr<Tree>
sexpr(const Tree* t) { return  print_sexpr<Tree>{t}; }

// Allows token to be printed as an sexpr.
inline print_sexpr<Token>
sexpr(const Token* t) { return  print_sexpr<Token>{t}; }

// Allows sequences to be printed as an sexpr.
template<typename T>
  inline print_sexpr<Tree_sequence<T>>
  sexpr(const Tree_sequence<T>& s) { return print_sexpr<Tree_sequence<T>>{s}; }

void to_sexpr(std::ostream& os, const Tree*);
void to_sexpr(std::ostream& os, const Token*);

template<typename U>
  std::ostream&
  operator<<(std::ostream& os, const print_sexpr<U>& x) {
    to_sexpr(os, x.thing);
    return os;
  }

template<typename T>
  inline void 
  to_sexpr(std::ostream& os, const Tree_sequence<T>& seq) {
    for (const Tree* x : seq)
      os << sexpr(x);
  }

} // namespace steve

#endif
