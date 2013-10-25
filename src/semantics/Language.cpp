
#include <cassert>
#include <iostream>

#include "Language.hpp"

namespace sarah {


// -------------------------------------------------------------------------- //
// Type inference

const Type&
type_int(Context& cxt, const Int& e) {
  return cxt.int_type;
}

const Type&
type_var(Context& cxt, const Var& e) {
  return cxt.int_type;
}

/*
const Type*
infer_unit_expr(const Stack& stk, const Unit_expr& e) {
  return &global_scope(stk).unit_type();
}

const Type*
infer_bool_expr(const Stack& stk, const Bool_expr& e) {
  return &global_scope(stk).bool_type();
}

const Type*
infer_nat_expr(const Stack& stk, const Nat_expr& e) {
  return &global_scope(stk).nat_type();
}

const Type*
infer_int_expr(const Stack& stk, const Int_expr& e) {
  return &global_scope(stk).int_type();
}

const Type*
infer_char_expr(const Stack& stk, const Char_expr& e) {
  return &global_scope(stk).char_type();
}

const Type*
infer_sting_expr(const Stack& stk, const String_expr& e) {
  return &global_scope(stk).string_type();
}

const Type*
infer_byte_expr(const Stack& stk, const Byte_expr& e) {
  return &global_scope(stk).byte_type();
}

const Type*
infer_addr_expr(const Stack& stk, const Addr_expr& e) {
  return &global_scope(stk).addr_type();
}

//
// Type checking
//

// Returns true e has type type *. This is the case whenever the
// dynamic type of e is a subclass of Type.
bool
check_type(Stack&, const Expr& e) { return is<Type>(e); }

// Returns true. This is trivially true for any expression whose
// dynamic type is a subclass of Type.
bool
check_type(Stack&, const Type& t) { return true; }

// Returns true when the const type is well-formed.
bool
check_const(Stack& stk, const Const_type& t) {
  return check_type(stk, t.type());
}

// Returns true when the ref type is well-formed.
bool
check_const(Stack& stk, const Ref_type& t) {
  return check_type(stk, t.type());
}
*/

//
// Visitor
//

void 
Expr::Visitor::visit_expr(const Expr& e) { }

void
Expr::Visitor::visit(const Int& n) { visit_expr(n); }

void
Expr::Visitor::visit(const Var& v) { visit_expr(v); }

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
Expr::Visitor::visit(const Exists& e) { visit_expr(e); }

void
Expr::Visitor::visit(const Forall& e) { visit_expr(e); }

void 
Expr::Visitor::visit_type(const Type& t) { visit_expr(t); }

void 
Expr::Visitor::visit(const Bool_type& t) { visit_type(t); }

void 
Expr::Visitor::visit(const Int_type& t) { visit_type(t); }

//
// Factory
//

Int&
Expr::Factory::make_int(Integer n) { return ints.make(n); }

Var&
Expr::Factory::make_var(String s) { return vars.make(s); }

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

Exists&
Expr::Factory::make_exists(const Var& n, const Expr& e) {
  return exs.make(n, e);
}

Forall&
Expr::Factory::make_forall(const Var& n, const Expr& e) {
  return fas.make(n, e);
}

Bool_type&
Expr::Factory::make_bool_type() { return bool_type.make(); }

Int_type&
Expr::Factory::make_int_type() { return int_type.make(); }

//
// Context
//

Context::Context() 
  : bool_type(make_bool_type())
  , int_type(make_int_type())
{ }

Context::~Context() {
}


} // namespace sarah
