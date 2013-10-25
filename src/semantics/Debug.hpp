
#ifndef SARAH_DEBUG_HPP
#define SARAH_DEBUG_HPP

#include <utility/Ios.hpp>

namespace sarah {

struct Expr;

// Debugging interface
void print_expr(std::ostream&, const Expr&);

// Streaming interface
template<typename C, typename T>
  inline std::basic_ostream<C, T>& 
  operator<<(std::basic_ostream<C, T>& os, const Expr& e) {
    print_expr(os, e);
    return os;
  }

} // namespace sarah

#endif
