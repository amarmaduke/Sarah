
#include <iostream>

#include "Sexpr.hpp"
#include "Tree.hpp"

namespace sarah {

namespace {

template<typename T>
  void 
  do_sexpr(std::ostream& os, const Structure<T>& t) {
    os << sexpr(t.first());
  }

template<typename T1, typename T2>
  void 
  do_sexpr(std::ostream& os, const Structure<T1, T2>& t) {
    os << '(' << sexpr(t.first())
       << ' ' << sexpr(t.second()) << ')';
  }

template<typename T1, typename T2, typename T3>
  inline void 
  do_sexpr(std::ostream& os, const Structure<T1, T2, T3>& t) {
    os << '(' << sexpr(t.first())
       << ' ' << sexpr(t.second()) 
       << ' ' << sexpr(t.third()) << ')';
  }

void
do_sexpr(std::ostream& os, const Enclosed_tree& t) {
  os << sexpr(t.arg());
}

} // namespace

// Writes the spelling of a token to the output stream.
std::ostream&
to_sexpr(std::ostream& os, const Token& tok) {
  return os << tok.spell;
}

// Writes the tree t to an output stream.
std::ostream&
to_sexpr(std::ostream& os, const Tree& ast) {
  struct V : Tree::Visitor {
    V(std::ostream& os) : os(os) { }
    void visit(const Enclosed_tree& t) { do_sexpr(os, t); }
    void visit(const Identifier_tree& t) { do_sexpr(os, t); }
    void visit(const Literal_tree& t) { do_sexpr(os, t); }
    void visit(const Unary_tree& t) { do_sexpr(os, t); }
    void visit(const Binary_tree& t) { do_sexpr(os, t); }
    std::ostream& os;
  };

  V v(os);
  ast.accept(v);
  return os;
}

} // namespace sarah
