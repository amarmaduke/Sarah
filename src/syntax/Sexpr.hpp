
#ifndef SARAH_SEXPR_HPP
#define SARAH_SEXPR_HPP

#include <iosfwd>

namespace sarah {

struct Token;
struct Tree;

// Stream as sexpr
std::ostream& to_sexpr(std::ostream&, const Tree&);
std::ostream& to_sexpr(std::ostream&, const Token&);

// Sexpr-formatted output.
template<typename T>
  struct print_sexpr {
    print_sexpr(const T& t) : thing(t) { }
    const T& thing;
  };

inline print_sexpr<Tree>
sexpr(const Tree& t) { return  print_sexpr<Tree>{t}; }

inline print_sexpr<Token>
sexpr(const Token& t) { return  print_sexpr<Token>{t}; }

// Streaming
template<typename C, typename T, typename U>
  inline std::basic_ostream<C, T>&
  operator<<(std::basic_ostream<C, T>& os, const print_sexpr<U>& x) {
    return to_sexpr(os, x.thing);
  }

} // namespace sarahg

#endif
