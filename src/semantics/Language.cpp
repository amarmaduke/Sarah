
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
Environment::lookup(String str) {
  auto i = find(str);
  if (i == end())
    return nullptr;
  else
    return i->second;
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
Expr::Visitor::visit(const Add& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Neg& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Mul& e) { visit_expr(e); }

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

Add&
Expr::Factory::make_add(const Expr& l, const Expr& r) {
  return adds.make(l, r);
}

Neg&
Expr::Factory::make_neg(const Expr& e) { return negs.make(e); }

Mul&
Expr::Factory::make_mul(const Int& n, const Expr& e) { 
  return muls.make(n, e);
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

Not&
Expr::Factory::make_not(const Expr& e) { return nots.make(e); }

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
  stack.push(top);
  top.define(make_id("bool"), kind_type, bool_type);
  top.define(make_id("int"), kind_type, int_type);
}

Context::~Context() {
  stack.pop();
}


} // namespace sarah
