
#include <iostream>
#include <iomanip>

#include "Debug.hpp"
#include "Language.hpp"

namespace sarah {

namespace {

inline void
print_symbol(std::ostream& os, char c) { os << c; }

inline void
print_symbol(std::ostream& os, const char* str) { os << str; }

template<typename T>
  void
  print_atom(std::ostream& os, const Atom<T>& e) {
    os << e.first();
  }

template<typename T>
  void
  print_structure(std::ostream& os, const Structure<T>& e) {
    print_expr(os, e.first());
  }

template<typename T1, typename T2>
  void
  print_structure(std::ostream& os, const Structure<T1, T2>& e) {
    print_expr(os, e.first());
    print_symbol(os, ',');
    print_symbol(os, ' ');
    print_expr(os, e.second());
  }

template<typename T>
  void
  print_node(std::ostream& os, const char* str, const T& e) {
    print_symbol(os, str);
    print_symbol(os, '(');
    print_structure(os, e);
    print_symbol(os, ')');
  }

} // namespace

void
print_expr(std::ostream& os, const Expr& e) {
  struct V : Expr::Visitor {
    V(std::ostream& os)
      : os(os) { }

    void visit(const Id& n) { print_atom(os, n); }
    void visit(const Bool& e) { print_atom(os, e); }
    void visit(const Int& e) { print_atom(os, e); }
    void visit(const Var& e) { print_expr(os, e.name()); }

    // Arithmetic expressions
    void visit(const Add& e) { print_node(os, "add", e); }
    void visit(const Sub& e) { print_node(os, "sub", e); }
    void visit(const Mul& e) { print_node(os, "mul", e); }
    void visit(const Div& e) { print_node(os, "div", e); }
    void visit(const Neg& e) { print_node(os, "neg", e); }
    void visit(const Pos& e) { print_node(os, "pos", e); }

    // Relational expressions
    void visit(const Eq& e) { print_node(os, "eq", e); }
    void visit(const Ne& e) { print_node(os, "ne", e); }
    void visit(const Lt& e) { print_node(os, "lt", e); }
    void visit(const Gt& e) { print_node(os, "gt", e); }
    void visit(const Le& e) { print_node(os, "le", e); }
    void visit(const Ge& e) { print_node(os, "ge", e); }

    // Logical expressions
    void visit(const And& e) { print_node(os, "and", e); }
    void visit(const Or& e) { print_node(os, "or", e); }
    void visit(const Imp& e) { print_node(os, "imp", e); }
    void visit(const Iff& e) { print_node(os, "iff", e); }
    void visit(const Not& e) { print_node(os, "not", e); }

    void visit(const Bind& e) { print_node(os, "bind", e); }
    void visit(const Forall& e) { print_node(os, "forall", e); }
    void visit(const Exists& e) { print_node(os, "exists", e); }

    void visit(const Bool_type& t) { print_symbol(os, "bool"); }
    void visit(const Int_type& t) { print_symbol(os, "int"); }

    std::ostream& os;
  };

  // Print bools as true.
  os << std::boolalpha;

  V v(os);
  e.accept(v);
}


} // namespace sarah
