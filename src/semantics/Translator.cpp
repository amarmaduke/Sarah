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
translate_id(Translator& t, const Id& expr) {
  return Elaboration();
//  return t.context.make_id(expr.str());
}

Elaboration
translate_bool(Translator& t, const Bool& expr) {
  return { t.context.make_bool(expr.value()), t.context.bool_type };
}

Elaboration
translate_int(Translator& t, const Int& expr) {
  return { t.context.make_int(expr.value()), t.context.int_type };
}

Elaboration
translate_var(Translator& t, const Var& expr) {
  return Elaboration();
}

Elaboration
translate_add(Translator& t, const Add& expr) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  return { t.context.make_add(e1.expr(),e2.expr()), t.context.int_type };
}

Elaboration
translate_sub(Translator& t, const Sub& expr) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  return { t.context.make_add(e1.expr(),e2.expr()), t.context.int_type };
}

Elaboration
translate_mul(Translator& t, const Mul& expr) {
  Elaboration e1 = t(expr.first());
  Elaboration e2 = t(expr.second());
  return { t.context.make_add(e1.expr(),e2.expr()), t.context.int_type };
}

Elaboration
translate_div(Translator& t, const Div& expr) {
  Elaboration e1 = t(expr.first());
  Elaboration e2 = t(expr.second());
  return { t.context.make_add(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_neg(Translator& t, const Neg& expr) {
  Elaboration e = t(expr.arg());
  return { t.context.make_neg(e.expr()), t.context.int_type };
}

Elaboration
translate_pos(Translator& t, const Pos& expr) {
  Elaboration e = t(expr.arg());
  return { t.context.make_pos(e.expr()), t.context.int_type };
}

Elaboration
translate_eq(Translator& t, const Eq& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_ne(e1.expr(),e2.expr()), t.context.bool_type };
  else
    return { t.context.make_eq(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_ne(Translator& t, const Ne& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_eq(e1.expr(),e2.expr()), t.context.bool_type };
  else
    return { t.context.make_ne(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_lt(Translator& t, const Lt& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_ge(e1.expr(),e2.expr()), t.context.bool_type };
  else
    return { t.context.make_lt(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_gt(Translator& t, const Gt& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_le(e1.expr(),e2.expr()), t.context.bool_type };
  else
    return { t.context.make_gt(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_le(Translator& t, const Le& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_gt(e1.expr(),e2.expr()), t.context.bool_type };
  else
    return { t.context.make_le(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_ge(Translator& t, const Ge& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_lt(e1.expr(),e2.expr()), t.context.bool_type };
  else
    return { t.context.make_ge(e1.expr(),e2.expr()), t.context.bool_type };
}

Elaboration
translate_and(Translator& t, const And& expr, bool carrying_not) {
  if (carrying_not) {
    Elaboration e1 = t.translate(expr.left(),true);
    Elaboration e2 = t.translate(expr.right(),true);
    return { t.context.make_or(e1.expr(),e2.expr()), t.context.bool_type };
  } else {
    Elaboration e1 = t(expr.left());
    Elaboration e2 = t(expr.right());
    return { t.context.make_and(e1.expr(),e2.expr()), t.context.bool_type };
  }
}

Elaboration
translate_or(Translator& t, const Or& expr, bool carrying_not) {
  if (carrying_not) {
    Elaboration e1 = t.translate(expr.left(),true);
    Elaboration e2 = t.translate(expr.right(),true);
    return { t.context.make_and(e1.expr(),e2.expr()), t.context.bool_type };
  } else {
    Elaboration e1 = t(expr.left());
    Elaboration e2 = t(expr.right());
    return { t.context.make_or(e1.expr(),e2.expr()), t.context.bool_type };
  }
}

Elaboration
translate_imp(Translator& t, const Imp& expr, bool carrying_not) {
  if (carrying_not) {
    Elaboration e1 = t.translate(expr.left(),false);
    Elaboration e2 = t.translate(expr.right(),true);
    return { t.context.make_and(e1.expr(),e2.expr()), t.context.bool_type };
  } else {
    Elaboration e1 = t.translate(expr.left(),true);
    Elaboration e2 = t.translate(expr.right(),false);
    return { t.context.make_or(e1.expr(),e2.expr()), t.context.bool_type };
  }
}

Elaboration
translate_iff(Translator& t, const Iff& expr, bool carrying_not) {
  Elaboration e1 = t(expr.left());
  Elaboration e2 = t(expr.right());
  if (carrying_not)
    return { t.context.make_gt(e1.expr(),e2.expr()) };
  else
    return { t.context.make_le(e1.expr(),e2.expr()) };
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
Translator::translate(const Expr& expr, bool carrying_not) {
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

    void visit(const Add& expr) { result = translate_add(translator,expr); }
    void visit(const Sub& expr) { result = translate_sub(translator,expr); }
    void visit(const Mul& expr) { result = translate_mul(translator,expr); }
    void visit(const Div& expr) { result = translate_div(translator,expr); }
    void visit(const Neg& expr) { result = translate_neg(translator,expr); }
    void visit(const Pos& expr) { result = translate_pos(translator,expr); }

    void visit(const Eq& expr) { result = translate_eq(translator,expr,false); }
    void visit(const Ne& expr) { result = translate_ne(translator,expr,false); }
    void visit(const Lt& expr) { result = translate_lt(translator,expr,false); }
    void visit(const Gt& expr) { result = translate_gt(translator,expr,false); }
    void visit(const Le& expr) { result = translate_le(translator,expr,false); }
    void visit(const Ge& expr) { result = translate_ge(translator,expr,false); }

    void visit(const And& expr) { result = translate_and(translator,expr,false); }
    void visit(const Or& expr) { result = translate_or(translator,expr,false); }
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

Elaboration
Translator::operator()(const Expr& expr) {
  return this->translate(expr,false);
}

} // namespace sarah
