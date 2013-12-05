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
  if (const Decl* d = t.context.lookup(expr.str()))
    return { t.context.make_id(expr.str()), d->type };
  return Elaboration();
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
  return { t.context.make_var(expr.name(),expr.decl()), expr.decl().type };
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
  return { t.context.make_sub(e1.expr(),e2.expr()), t.context.int_type };
}

Elaboration
translate_mul(Translator& t, const Mul& expr) {
  Elaboration e1 = t(expr.first());
  Elaboration e2 = t(expr.second());
  return {
    t.context.make_mul(as<Int>(e1.expr()),e2.expr()), t.context.int_type
  };
}

Elaboration
translate_div(Translator& t, const Div& expr, bool carrying_not) {
  Elaboration e1 = t(expr.first());
  Elaboration e2 = t(expr.second());
  Elaboration e = {
    t.context.make_div(as<Int>(e1.expr()),e2.expr()),
    t.context.bool_type
  };
  if (carrying_not ) {
    return { t.context.make_not(e.expr()), t.context.bool_type };
  } else {
    return e;
  }
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
  Elaboration e1 = t.translate(expr.left(),true);
  Elaboration e2 = t.translate(expr.left(),false);
  Elaboration e3 = t.translate(expr.right(),true);
  Elaboration e4 = t.translate(expr.right(),false);

  if (carrying_not) {
    Elaboration left = {
      t.context.make_and(e2.expr(),e3.expr()),
      t.context.bool_type
    };
    Elaboration right = {
      t.context.make_and(e4.expr(),e1.expr()),
      t.context.bool_type
    };
    return { t.context.make_or(left.expr(),right.expr()), t.context.bool_type };
  } else {
    Elaboration left = {
      t.context.make_or(e1.expr(),e4.expr()),
      t.context.bool_type
    };
    Elaboration right = {
      t.context.make_or(e3.expr(),e2.expr()),
      t.context.bool_type
    };
    return { t.context.make_and(left.expr(),right.expr()), t.context.bool_type};
  }
}


Elaboration
translate_not(Translator& t, const Not& expr, bool carrying_not) {
  if (carrying_not)
  {
    Elaboration e = t(expr.arg());
    return e;
  } else {
    Elaboration e = t.translate(expr.arg(),true);
    return e;
  }
}

Elaboration
translate_bind(Translator& t, const Bind& expr) {
  return { t.context.make_bind(expr.name(), expr.type()), expr.type() };
}

Elaboration
translate_exists(Translator& t, const Exists& expr, bool carrying_not) {
  Elaboration e1 = t(expr.binding());
  if (carrying_not)
  {
    Elaboration e2 = t.translate(expr.expr(),true);
    return {
      t.context.make_forall(as<Bind>(e1.expr()),e2.expr()), t.context.bool_type
    };
  } else {
    Elaboration e2 = t.translate(expr.expr(),false);
    return {
      t.context.make_exists(as<Bind>(e1.expr()),e2.expr()), t.context.bool_type
    };
  }
}

Elaboration
translate_forall(Translator& t, const Forall& expr, bool carrying_not) {
  Elaboration e1 = t(expr.binding());
  if (carrying_not)
  {
    Elaboration e2 = t.translate(expr.expr(),true);
    return {
      t.context.make_exists(as<Bind>(e1.expr()),e2.expr()), t.context.bool_type
    };
  } else {
    Elaboration e2 = t.translate(expr.expr(),false);
    return {
      t.context.make_forall(as<Bind>(e1.expr()),e2.expr()), t.context.bool_type
    };
  }
}

Elaboration
translate_bool_type(Translator& t, const Bool_type& expr) {
  return { t.context.make_bool_type(), t.context.bool_type };
}

Elaboration
translate_int_type(Translator& t, const Int_type& expr) {
  return { t.context.make_int_type(), t.context.int_type };
}

Elaboration
translate_kind_type(Translator& t, const Kind_type& expr) {
  return { t.context.make_kind_type(), t.context.kind_type };
}

Elaboration
translate_type(Translator& t, const Expr& expr) {
  std::cout << "type" << std::endl;
  return Elaboration();
}

Elaboration
translate_expr(Translator& t, const Expr& expr) {
  std::cout << "expr" << std::endl;
  return Elaboration();
}

} // namespace

Elaboration
Translator::translate(const Expr& expr, bool carrying_not) {
  struct V : Expr::Visitor {
    V(Translator& t, bool cn)
      : translator(t), carrying_not(cn) { }

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
    void visit(const Neg& expr) { result = translate_neg(translator,expr); }
    void visit(const Pos& expr) { result = translate_pos(translator,expr); }


    void visit(const Div& expr) {
      result = translate_div(translator,expr,carrying_not);
    }
    void visit(const Eq& expr) {
      result = translate_eq(translator,expr,carrying_not);
    }
    void visit(const Ne& expr) {
      result = translate_ne(translator,expr,carrying_not);
    }
    void visit(const Lt& expr) {
      result = translate_lt(translator,expr,carrying_not);
    }
    void visit(const Gt& expr) {
      result = translate_gt(translator,expr,carrying_not);
    }
    void visit(const Le& expr) {
      result = translate_le(translator,expr,carrying_not);
    }
    void visit(const Ge& expr) {
      result = translate_ge(translator,expr,carrying_not);
    }

    void visit(const And& expr) {
      result = translate_and(translator,expr,carrying_not);
    }
    void visit(const Or& expr) {
      result = translate_or(translator,expr,carrying_not);
    }
    void visit(const Imp& expr) {
      result = translate_imp(translator,expr,carrying_not);
    }
    void visit(const Iff& expr) {
      result = translate_iff(translator,expr,carrying_not);
    }
    void visit(const Not& expr) {
      result = translate_not(translator,expr,carrying_not);
    }
    void visit(const Bind& expr) {
      result = translate_bind(translator,expr);
    }

    void visit(const Exists& expr) {
      result = translate_exists(translator,expr,carrying_not);
    }
    void visit(const Forall& expr) {
      result = translate_forall(translator,expr,carrying_not);
    }

    void visit_type(const Type& expr) {
      result = translate_type(translator,expr);
    }
    void visit(const Bool_type& expr) {
      result = translate_type(translator,expr);
    }
    void visit(const Int_type& expr) {
      result = translate_type(translator,expr);
    }
    void visit(const Kind_type& expr) {
      result = translate_type(translator,expr);
    }

    Translator& translator;
    Elaboration result;
    bool carrying_not;
  };

  V vis(*this,carrying_not);
  expr.accept(vis);
  return vis.result;
}

Elaboration
Translator::operator()(const Expr& expr) {
  return this->translate(expr,false);
}

} // namespace sarah
