
#include <iostream>

#include "Sexpr.hpp"
#include "Ast.hpp"

namespace steve {

namespace {

template<typename Seq, typename Punc = char>
  void intercalate(std::ostream& os, const Seq& seq, Punc punc = ' ') {
    auto i = seq.begin();
    auto e = seq.end();
    while (i != e) {
      os << sexpr(*i);
      if (std::next(i) != e)
        os << punc;
      ++i;
    }
  }

template<typename D, typename T>
  inline void 
  to_sexpr(std::ostream& os, const Tree_struct<D, T>* t) {
    os << sexpr(t->first());
  }

template<typename D, typename T1, typename T2>
  inline void 
  to_sexpr(std::ostream& os, const Tree_struct<D, T1, T2>* t) {
    os << '(' << sexpr(t->first())
       << ' ' << sexpr(t->second()) << ')';
  }

template<typename D, typename T1, typename T2, typename T3>
  inline void 
  to_sexpr(std::ostream& os, const Tree_struct<D, T1, T2, T3>* t) {
    os << '(' << sexpr(t->first())
       << ' ' << sexpr(t->second()) 
       << ' ' << sexpr(t->third()) << ')';
  }

inline void
to_sexpr(std::ostream& os, const Enclosed_tree* t) {
  os << sexpr(t->arg());
}

void
to_sexpr(std::ostream& os, const Sequence_tree* t) {
  os << '(';
  intercalate(os, *t);
  os << ')';
}

inline void
to_sexpr(std::ostream& os, const Call_tree* t) {
  os << "(call " << sexpr(t->target()) << ' ' << sexpr(t->args()) << ')';
}

inline void
to_sexpr(std::ostream& os, const Access_tree* t) {
  os << "(access " << sexpr(t->scope()) << ' ' << sexpr(t->member()) << ')';
}

inline void
to_sexpr(std::ostream& os, const Index_tree* t) {
  os << "(index " << sexpr(t->store()) << ' ' << sexpr(t->index()) << ')';
}

inline void
to_sexpr(std::ostream& os, const Def_tree* t) {
  os << "(def " << sexpr(t->decl()) 
     << ' '     << sexpr(t->type())
     << ' '     << sexpr(t->init()) << ')';
}

inline void
to_sexpr(std::ostream& os, const Scan_tree* t) {
  os << "(scan " << sexpr(t->name()) << ' ' << sexpr(t->type()) << ')';
}
inline void
to_sexpr(std::ostream& os, const Print_tree* t) {
  os << "(print " << sexpr(t->arg()) << ')';
}

inline void
to_sexpr(std::ostream& os, const Program_tree* t) {
  // TODO: It would be nice to have an indent, but that would require
  // more pretty printing support.
  os << "(program" << '\n';
  intercalate(os, t->stmts(), '\n');
  os << '\n' << ')';
}


} // namespace

// Writes the spelling of a token to the output stream.
void
to_sexpr(std::ostream& os, const Token* tok) {
  os << tok->spell;
}

// Writes the tree t to an output stream.
void 
to_sexpr(std::ostream& os, const Tree* t) {
  struct V : Tree::Visitor {
    V(std::ostream& os) : os(os) { }
    void visit(const Enclosed_tree* t) { to_sexpr(os, t); }
    void visit(const Sequence_tree* t) { to_sexpr(os, t); }
    void visit(const Identifier_tree* t) { to_sexpr(os, t); }
    void visit(const Literal_tree* t) { to_sexpr(os, t); }
    void visit(const Unary_tree* t) { to_sexpr(os, t); }
    void visit(const Binary_tree* t) { to_sexpr(os, t); }
    void visit(const Call_tree* t) { to_sexpr(os, t); }
    void visit(const Access_tree* t) { to_sexpr(os, t); }
    void visit(const Index_tree* t) { to_sexpr(os, t); }
    void visit(const Type_tree* t) { to_sexpr(os, t); }
    void visit(const Value_tree* t) { to_sexpr(os, t); }
    void visit(const Function_tree* t) { to_sexpr(os, t); }
    void visit(const Template_tree* t) { to_sexpr(os, t); }
    void visit(const Parameter_tree* t) { to_sexpr(os, t); }
    void visit(const Def_tree* t) { to_sexpr(os, t); }
    void visit(const Scan_tree* t) { to_sexpr(os, t); }
    void visit(const Print_tree* t) { to_sexpr(os, t); }
    void visit(const Program_tree* t) { to_sexpr(os, t); }
    std::ostream& os;
  };

  V v(os);
  if (t)
    t->accept(v);
  else
    os << "(null)";
}

} // namespace steve
