
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
    print_expr(os, e.second());
  }

template<typename T>
  void
  print_node(std::ostream& os, const char* str, const T& e) {
    print_symbol(os, str);
    print_symbol(os, ' ');
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
    void visit(const Add& e) { print_node(os, "add", e); }
    void visit(const Neg& e) { print_node(os, "neg", e); }

    void visit(const Bind& e) { print_node(os, "bind", e); }
    
    void visit(const Forall& e) { print_node(os, "forall", e); }

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
