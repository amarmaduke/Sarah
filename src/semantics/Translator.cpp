// Working on identity translator to start off
#include <cassert>
#include <iostream>

#include "utility/Diagnostics.hpp"
#include "syntax/Tree.hpp"
#include "syntax/Sexpr.hpp"

#include "Translator.hpp"
#include "Elaborator.hpp"
#include "Language.hpp"
#include "Debug.hpp"

namespace sarah {

namespace {

Elaboration
translate_id(Translator& t, const Expr& expr) {
  std::cout << "id" << std::endl;
  return Elaboration();
}

Elaboration
translate_bool(Translator& t, const Expr& expr) {
  std::cout << "bool" << std::endl;
  return Elaboration();
}

Elaboration
translate_int(Translator& t, const Expr& expr) {
  std::cout << "int" << std::endl;
  return Elaboration();
}

Elaboration
translate_var(Translator& t, const Expr& expr) {
  std::cout << "var" << std::endl;
  return Elaboration();
}

Elaboration
translate_binary(Translator& t, const Expr& expr) {
  std::cout << "binary" << std::endl;
  return Elaboration();
}

Elaboration
translate_unary(Translator& t, const Expr& expr) {
  std::cout << "unary" << std::endl;
  return Elaboration();
}

Elaboration
translate_bind(Translator& t, const Expr& expr) {
  std::cout << "bind" << std::endl;
  return Elaboration();
}

Elaboration
translate_quantifier(Translator& t, const Expr& expr) {
  std::cout << "quantifier" << std::endl;
  return Elaboration();
}

Elaboration
translate_type(Translator& t, const Expr& expr) {
  std::cout << "type" << std::endl;
  return Elaboration();
}

Elaboration
translate_expr(Translator& t, const Expr& expr) {
  return Elaboration();
}

} // namespace

Elaboration
Translator::operator()(const Expr& expr) {
  struct V : Expr::Visitor {
    V(Translator& t)
      : translator(t) { }

    void visit_expr(const Expr& expr) {
      result = translate_expr(translator,expr);
    }

    void visit(const Id& expr) { result = translate_id(translator,expr); }
    void visit(const Bool& expr) { result = translate_bool(translator,expr); }
    void visit(const Int& expr) { result = translate_int(translator,expr); }
    void visit(const Var& expr) { result = translate_var(translator,expr); }

    void visit(const Add& expr) { result = translate_binary(translator,expr); }
    void visit(const Sub& expr) { result = translate_binary(translator,expr); }
    void visit(const Mul& expr) { result = translate_binary(translator,expr); }
    void visit(const Div& expr) { result = translate_binary(translator,expr); }
    void visit(const Neg& expr) { result = translate_unary(translator,expr); }
    void visit(const Pos& expr) { result = translate_unary(translator,expr); }

    void visit(const Eq& expr) { result = translate_binary(translator,expr); }
    void visit(const Ne& expr) { result = translate_binary(translator,expr); }
    void visit(const Lt& expr) { result = translate_binary(translator,expr); }
    void visit(const Gt& expr) { result = translate_binary(translator,expr); }
    void visit(const Le& expr) { result = translate_binary(translator,expr); }
    void visit(const Ge& expr) { result = translate_binary(translator,expr); }

    void visit(const And& expr) { result = translate_binary(translator,expr); }
    void visit(const Or& expr) { result = translate_binary(translator,expr); }
    void visit(const Imp& expr) { result = translate_binary(translator,expr); }
    void visit(const Iff& expr) { result = translate_binary(translator,expr); }
    void visit(const Not& expr) { result = translate_unary(translator,expr); }
    void visit(const Bind& expr) { result = translate_bind(translator,expr); }

    void visit(const Exists& expr) {
      result = translate_quantifier(translator,expr);
    }
    void visit(const Forall& expr) {
      result = translate_quantifier(translator,expr);
    }

    void visit_type(const Type& expr) { result = translate_type(translator,expr); }
    void visit(const Bool_type& expr) { result = translate_type(translator,expr); }
    void visit(const Int_type& expr) { result = translate_type(translator,expr); }
    void visit(const Kind_type& expr) { result = translate_type(translator,expr); }

    Translator& translator;
    Elaboration result;
  };

  V vis(*this);
  expr.accept(vis);
  return vis.result;
}

} // namespace sarah
