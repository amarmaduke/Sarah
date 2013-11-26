
#include <cassert>
#include <iostream>

#include "Language.hpp"

namespace sarah {

// -------------------------------------------------------------------------- //
// Environment

Environment::~Environment() {
  for(auto p : *this)
    delete p.second;
}

// Insert the declaration binding n : t into the environment.
const Decl&
Environment::declare(const Id& n, const Type& t) {
  assert(no_binding(n.str()));
  Decl* d = new Decl(n, t);
  insert({n.str(), d});
  return *d;
}

// Insert the definition binding n : t -> e into the environment.
const Def&
Environment::define(const Id& n, const Type& t, const Expr& e) {
  assert(no_binding(n.str()));
  Def *d = new Def(n, t, e);
  insert({n.str(), d});
  return *d;
}

// Return a pointer to the declaration binding indicated by str or
// nullptr if no such binding exists.
const Decl*
Environment::lookup(String str) const {
  auto i = find(str);
  if (i == end())
    return nullptr;
  else
    return i->second;
}

// -------------------------------------------------------------------------- //
// Stack
//
// TODO: The current binding environment may not permit arbitrary bindings.
// What happens if we try to add a binding to a quantifier? It shouldn't be
// allowed. Maybe we need a can_declare/can_define predicate that guarantees
// the viability of the operation.

// Add n : t to the current binding environment.
const Decl&
Stack::declare(const Id& n, const Type& t) {
  return top().declare(n, t);
}

// Add n : t -> e to the current binding environment.
const Def&
Stack::define(const Id& n, const Type& t, const Expr& e) {
  return top().define(n, t, e);
}

// Search the stack for the given name. The search walks down the stack,
// starting with the most recently pushed environment.
const Decl*
Stack::lookup(String s) const {
  for(auto i = rbegin(); i != rend(); ++i) {
    const Environment* env = *i;
    if (const Decl* d = env->lookup(s))
      return d;
  }
  return nullptr;
}

// -------------------------------------------------------------------------- //
// Expression equality

namespace {

inline bool
same_id(const Id& a, const Id& b) { return a.str() == b.str(); }

inline bool
same_bool(const Bool& a, const Bool& b) { return a.value() == b.value(); }

inline bool
same_int(const Int& a, const Int& b) { return a.value() == b.value(); }

inline bool
same_var(const Var& a, const Var& b) { return &a.decl() == &b.decl(); }

// Two types are the same only when they are the same object.
inline bool
same_type(const Type& a, const Type& b) { return &a == &b; }

// Here we're saying that two binary exprs are the same if they are identical
inline bool
same_binary(const Binary& a, const Binary& b) {
  return same(a.left(),b.left()) and same(a.right(),b.right());
}

inline bool
same_multiplicative(const Structure<Int,Expr>& a,
                    const Structure<Int,Expr>& b) {
  return same(a.first(),b.first()) and same(a.second(),b.second());
}

inline bool
same_bind(const Structure<Id,Type>& a, const Structure<Id,Type>& b) {
  return same(a.first(),b.first()) and same(a.second(),b.second());
}

inline bool
same_quantifier(const Structure<Bind,Expr>& a, const Structure<Bind,Expr>& b) {
  return same(a.first(),b.first()) and same(a.second(),b.second());
}

inline bool
same_unary(const Unary& a, const Unary& b) {
  return same(a.arg(),b.arg());
}

bool
same_expr(const Expr& a, const Expr& b) {
  struct V : Expr::Visitor {
    V(const Expr& e)
      : expr(e), result(false) { }

    void visit(const Id& e) { result = same_id(e, as<Id>(expr)); }
    void visit(const Bool& e) { result = same_bool(e, as<Bool>(expr)); }
    void visit(const Int& e) { result = same_int(e, as<Int>(expr)); }
    void visit(const Var& e) { result = same_var(e, as<Var>(expr)); }

    // TODO: Implement these functions.

    void visit(const Add& e) { result = same_binary(e,as<Add>(expr)); }
    void visit(const Sub& e) { result = same_binary(e,as<Sub>(expr)); }
    void visit(const Mul& e) { result = same_multiplicative(e,as<Mul>(expr)); }
    void visit(const Div& e) { result = same_multiplicative(e,as<Div>(expr)); }
    void visit(const Neg& e) { result = same_unary(e,as<Neg>(expr)); }
    void visit(const Pos& e) { result = same_unary(e,as<Pos>(expr));}

    void visit(const Eq& e) { result = same_binary(e,as<Eq>(expr)); }
    void visit(const Ne& e) { result = same_binary(e,as<Ne>(expr)); }
    void visit(const Lt& e) { result = same_binary(e,as<Lt>(expr)); }
    void visit(const Gt& e) { result = same_binary(e,as<Gt>(expr)); }
    void visit(const Le& e) { result = same_binary(e,as<Le>(expr)); }
    void visit(const Ge& e) { result = same_binary(e,as<Ge>(expr)); }

    void visit(const And& e) { result = same_binary(e,as<And>(expr)); }
    void visit(const Or& e) { result = same_binary(e,as<Or>(expr)); }
    void visit(const Imp& e) { result = same_binary(e,as<Imp>(expr)); }
    void visit(const Iff& e) { result = same_binary(e,as<Iff>(expr)); }
    void visit(const Not& e) { result = same_unary(e,as<Not>(expr));}
    void visit(const Bind& e) { result = same_bind(e,as<Bind>(expr));}

    void visit(const Exists& e) {
      result = same_quantifier(e,as<Exists>(expr));
    }
    void visit(const Forall& e) {
      result = same_quantifier(e,as<Forall>(expr));
    }

    void visit_type(const Type& t) { result = same_type(t, as<Type>(expr)); }

    const Expr& expr;
    bool result;
  };

  V vis(b);
  a.accept(vis);
  return vis.result;
}

} // namespace

bool
same(const Expr& a, const Expr& b) {
  if (kind(a) != kind(b))
    return false;
  return same_expr(a, b);
}


// -------------------------------------------------------------------------- //
// Visitor

void
Expr::Visitor::visit_expr(const Expr& e) { }

void
Expr::Visitor::visit(const Id& n) { visit_expr(n); }

void
Expr::Visitor::visit(const Bool& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Int& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Var& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Add& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Sub& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Mul& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Div& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Neg& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Pos& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Eq& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Ne& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Lt& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Gt& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Le& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Ge& e) { visit_expr(e); }

void
Expr::Visitor::visit(const And& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Or& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Imp& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Iff& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Not& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Bind& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Exists& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Forall& e) { visit_expr(e); }

void
Expr::Visitor::visit_type(const Type& t) { visit_expr(t); }

void
Expr::Visitor::visit(const Bool_type& t) { visit_type(t); }

void
Expr::Visitor::visit(const Int_type& t) { visit_type(t); }

void
Expr::Visitor::visit(const Kind_type& t) { visit_type(t); }

// -------------------------------------------------------------------------- //
// Factory

Id&
Expr::Factory::make_id(String s) { return ids.make(s); }

Bool&
Expr::Factory::make_bool(bool b) { return bools.make(b); }

Int&
Expr::Factory::make_int(Integer n) { return ints.make(n); }

Var&
Expr::Factory::make_var(const Id& n, const Decl& d) {
  return vars.make(n, d);
}

Add&
Expr::Factory::make_add(const Expr& l, const Expr& r) {
  return adds.make(l, r);
}

Sub&
Expr::Factory::make_sub(const Expr& l, const Expr& r) {
  return subs.make(l, r);
}

Mul&
Expr::Factory::make_mul(const Int& n, const Expr& e) {
  return muls.make(n, e);
}

Div&
Expr::Factory::make_div(const Int& n, const Expr& e) {
  return divs.make(n, e);
}

Neg&
Expr::Factory::make_neg(const Expr& e) {
  return negs.make(e);
}

Pos&
Expr::Factory::make_pos(const Expr& e) {
  return poss.make(e);
}

Eq&
Expr::Factory::make_eq(const Expr& l, const Expr& r) {
  return eqs.make(l, r);
}

Ne&
Expr::Factory::make_ne(const Expr& l, const Expr& r) {
  return nes.make(l, r);
}

Lt&
Expr::Factory::make_lt(const Expr& l, const Expr& r) {
  return lts.make(l, r);
}

Gt&
Expr::Factory::make_gt(const Expr& l, const Expr& r) {
  return gts.make(l, r);
}

Le&
Expr::Factory::make_le(const Expr& l, const Expr& r) {
  return les.make(l, r);
}

Ge&
Expr::Factory::make_ge(const Expr& l, const Expr& r) {
  return ges.make(l, r);
}

And&
Expr::Factory::make_and(const Expr& l, const Expr& r) {
  return ands.make(l, r);
}

Or&
Expr::Factory::make_or(const Expr& l, const Expr& r) {
  return ors.make(l, r);
}

Imp&
Expr::Factory::make_imp(const Expr& l, const Expr& r) {
  return imps.make(l, r);
}

Iff&
Expr::Factory::make_iff(const Expr& l, const Expr& r) {
  return iffs.make(l, r);
}

Not&
Expr::Factory::make_not(const Expr& e) {
  return nots.make(e);
}

Bind&
Expr::Factory::make_bind(const Id& n, const Type& t) {
  return binds.make(n, t);
}

Exists&
Expr::Factory::make_exists(const Bind& b, const Expr& e) {
  return exs.make(b, e);
}

Forall&
Expr::Factory::make_forall(const Bind& b, const Expr& e) {
  return fas.make(b, e);
}

Bool_type&
Expr::Factory::make_bool_type() { return bool_type; }

Int_type&
Expr::Factory::make_int_type() { return int_type; }

Kind_type&
Expr::Factory::make_kind_type() { return kind_type; }

// -------------------------------------------------------------------------- //
// Context

Context::Context()
  : bool_type(make_bool_type())
  , int_type(make_int_type())
  , kind_type(make_kind_type())
  , top()
{
  push(top);
  bool_def = &define(make_id("bool"), kind_type, bool_type);
  int_def = &define(make_id("int"), kind_type, int_type);
}

Context::~Context() {
  pop();
}


} // namespace sarah
