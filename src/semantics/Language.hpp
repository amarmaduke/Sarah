
#ifndef SARAH_LANGUAGE_HPP
#define SARAH_LANGUAGE_HPP

#include <map>
#include <stack>
#include <vector>

#include <utility/String.hpp>
#include <utility/Integer.hpp>
#include <utility/Structure.hpp>

namespace sarah {

struct Expr;
struct Id;
struct Bool;
struct Int;
struct Add;
struct Sub;
struct Mul;
struct Neg;
struct Pos;
struct Eq;
struct Ne;
struct Lt;
struct Gt;
struct Le;
struct Ge;
struct And;
struct Or;
struct Imp;
struct Iff;
struct Not;
struct Exists;
struct Forall;

// Types
struct Type;
struct Bool_type;
struct Int_type;
struct Kind_type;

// Misc.
struct Context;

// -------------------------------------------------------------------------- //
// Environment

// A declaration is a binding between a name and type.
struct Decl {
  Decl(const Id& n, const Type& t)
    : name(n), type(t) { }

  virtual ~Decl() { }

  const Id& name;
  const Type& type;
};

// A definition is a declaration that includes an initializer.
struct Def : Decl {
  Def(const Id& n, const Type& t, const Expr& e)
    : Decl(n, t), init(e) { }

  const Expr& init;
};

// An Environment is a mapping of identifiers (strings) to declarations.
//
// TODO: Should the environment manage its own memory or use a factory.
// It currently does, but we could also provide local factories for
// Decls and Defs.
struct Environment : std::map<String, Decl*> {
  ~Environment();

  // Binding interface
  const Decl& declare(const Id&, const Type&);
  const Def& define(const Id&, const Type&, const Expr&);
  
  // Symbol lookup
  const Decl* lookup(String) const;
  bool has_binding(String s) { return lookup(s); }
  bool no_binding(String s)  { return not has_binding(s); }
};

// The stack is a stack of environments.
struct Stack : std::vector<Environment*> {
  using Base = std::vector<Environment*>;

  // Stack interface
  Environment& top() { return *Base::back(); }
  void push(Environment& e) { Base::push_back(&e); }
  void pop() { Base::pop_back(); }

  // Binding interface
  const Decl& declare(const Id&, const Type&);
  const Def& define(const Id&, const Type&, const Expr&);

  // Symbol lookup
  const Decl* lookup(String) const;
  const bool has_binding(String s) const { return lookup(s); }
  const bool no_binding(String s) const { return not has_binding(s); }
};

// -------------------------------------------------------------------------- //
// Expressions
//
// TODO: I think we may need a Var expression that, when typed will refer
// to its own binding.

// The Expr class is the base class of all expressions in the abstract
// language.
struct Expr {
  struct Visitor;
  struct Factory;

  virtual ~Expr() { }

  virtual void accept(Visitor&) const = 0;
};

// Expression interface
bool same(const Expr&, const Expr&);


// A helper class for expr implementations. The B parameter indicates
// the direct base of the implementing class, and D is the derived class.
template<typename D, typename B = Expr>
  struct Expr_impl : B {
    template<typename... Args>
      Expr_impl(Args&&... args)
        : B(std::forward<Args>(args)...) { }

    virtual void accept(typename B::Visitor& v) const;
  };

// An identifier denoting a binding.
struct Id : Atom<String>, Expr_impl<Id> {
  Id(String s)
    : Atom<String>(s) { }

  String str() const { return first(); }
};


// An integer literal. 
struct Int : Atom<Integer>, Expr_impl<Int> {
  Int(Integer n)
    : Atom<Integer>(n) { }
  
  const Integer& value() const { return first(); }
};

// A boolean literal.
struct Bool : Atom<bool>, Expr_impl<Bool> {
  Bool(bool b)
    : Atom<bool>(b) { }

  bool value() const { return first(); }
};

// A named reference to a binding.
struct Var : Structure<Id, Decl>, Expr_impl<Var> {
  Var(const Id& n, const Decl& d)
    : Structure<Id, Decl>(n, d) { }

  const Id& name() const { return first(); }
  const Decl& decl() const { return second(); }
};

// A base class for generic unary expressions.
struct Unary : Structure<Expr>, Expr {
  Unary(const Expr& l)
    : Structure<Expr>(l) { }

  const Expr& arg() const { return first(); }
};

template<typename D>
  using Unary_impl = Expr_impl<D, Unary>;

// A base class for generic binary expressions.
struct Binary : Structure<Expr, Expr>, Expr {
  Binary(const Expr& l, const Expr& r)
    : Structure<Expr, Expr>(l, r) { }

  const Expr& left() const { return first(); }
  const Expr& right() const { return second(); }
};

template<typename D>
  using Binary_impl = Expr_impl<D, Binary>;


// Addition, `e1 - e2`
struct Add : Binary_impl<Add> {
  Add(const Expr& l, const Expr& r)
    : Binary_impl<Add>(l, r) { }
};

// Subtraction.
struct Sub : Binary_impl<Sub> {
  Sub(const Expr& l, const Expr& r)
    : Binary_impl<Sub>(l, r) { }
};

// Repeated addition. 
struct Mul : Structure<Int, Expr>, Expr_impl<Mul> {
  Mul(const Int& n, const Expr& e)
    : Structure<Int, Expr>(n, e) { }
};

// Arithmetic negation, `-z`.
struct Neg : Unary_impl<Neg> {
  Neg(const Expr& e)
    : Unary_impl<Neg>(e) { }
};

// Arithmetic identity, `+z`.
struct Pos : Unary_impl<Pos> {
  Pos(const Expr& e)
    : Unary_impl<Pos>(e) { }
};

// Equal.
struct Eq : Binary_impl<Eq> {
  Eq(const Expr& l, const Expr& r)
    : Binary_impl<Eq>(l, r) { }
};

// Not equal.
struct Ne : Binary_impl<Ne> {
  Ne(const Expr& l, const Expr& r)
    : Binary_impl<Ne>(l, r) { }
};

// Less than.
struct Lt : Binary_impl<Lt> {
  Lt(const Expr& l, const Expr& r)
    : Binary_impl<Lt>(l, r) { }
};

// Greater than.
struct Gt : Binary_impl<Gt> {
  Gt(const Expr& l, const Expr& r)
    : Binary_impl<Gt>(l, r) { }
};

// Less equal.
struct Le : Binary_impl<Le> {
  Le(const Expr& l, const Expr& r)
    : Binary_impl<Le>(l, r) { }
};

// Greater equal.
struct Ge : Binary_impl<Ge> {
  Ge(const Expr& l, const Expr& r)
    : Binary_impl<Ge>(l, r) { }
};

// Logical and.
struct And : Binary_impl<And> {
  And(const Expr& l, const Expr& r)
    : Binary_impl<And>(l, r) { }
};

// Logical or.
struct Or : Binary_impl<Or> {
  Or(const Expr& l, const Expr& r)
    : Binary_impl<Or>(l, r) { }
};

// Implication.
struct Imp : Binary_impl<Imp> {
  Imp(const Expr& l, const Expr& r)
    : Binary_impl<Imp>(l, r) { }
};

// If and only if.
struct Iff : Binary_impl<Iff> {
  Iff(const Expr& l, const Expr& r)
    : Binary_impl<Iff>(l, r) { }
};

// Logical not.
struct Not : Unary_impl<Not> {
  Not(const Expr& e)
    : Unary_impl<Not>(e) { }
};

// A name/type binding of the form n : t.
struct Bind : Structure<Id, Type>, Expr_impl<Bind> {
  Bind(const Id& n, const Type& t)
    : Structure<Id, Type>(n, t) { }

  const Id& name() const { return first(); }
  const Type& type() const { return second(); }
};

// There exits.
struct Exists : Structure<Bind, Expr>, Expr_impl<Exists> {
  Exists(const Bind& b, const Expr& e)
    : Structure<Bind, Expr>(b, e) { }

  const Bind& binding() const { return first(); }
  const Expr& expr() const { return second(); }
};

// Forall.
struct Forall : Structure<Bind, Expr>, Expr_impl<Forall> {
  Forall(const Bind& b, const Expr& e)
    : Structure<Bind, Expr>(b, e) { }

  const Bind& binding() const { return first(); }
  const Expr& expr() const { return second(); }
};


// -------------------------------------------------------------------------- //
// Types

struct Type : Expr { };

template<typename D>
  using Type_impl = Expr_impl<D, Type>;

// The bool type.
struct Bool_type : Type_impl<Bool_type> { };

// The int type.
struct Int_type : Type_impl<Int_type> { };

// The kind type (the type of all types).
struct Kind_type : Type_impl<Kind_type> { };

// -------------------------------------------------------------------------- //
// Visitor

struct Expr::Visitor {
  virtual void visit_expr(const Expr&);
  virtual void visit(const Id&);
  virtual void visit(const Bool&);
  virtual void visit(const Int&);
  virtual void visit(const Var&);

  virtual void visit(const Add&);
  virtual void visit(const Sub&);
  virtual void visit(const Mul&);
  virtual void visit(const Neg&);
  virtual void visit(const Pos&);

  virtual void visit(const Eq&);
  virtual void visit(const Ne&);
  virtual void visit(const Lt&);
  virtual void visit(const Gt&);
  virtual void visit(const Le&);
  virtual void visit(const Ge&);

  virtual void visit(const And&);
  virtual void visit(const Or&);
  virtual void visit(const Imp&);
  virtual void visit(const Iff&);
  virtual void visit(const Not&);
  virtual void visit(const Bind&);

  virtual void visit(const Exists&);
  virtual void visit(const Forall&);

  virtual void visit_type(const Type&);
  virtual void visit(const Bool_type&);
  virtual void visit(const Int_type&);
  virtual void visit(const Kind_type&);
};

template<typename D, typename B>
  inline void
  Expr_impl<D, B>::accept(typename B::Visitor& v) const {
    v.visit(static_cast<const D&>(*this));
  }

// -------------------------------------------------------------------------- //
// Factory

// Creates and stores expressions.
struct Expr::Factory {
  // Atomic expressions
  Id& make_id(String);
  Bool& make_bool(bool);
  Int& make_int(Integer);
  Var& make_var(const Id&, const Decl&);

  // Arithmetic expressions.
  Add& make_add(const Expr&, const Expr&);
  Sub& make_sub(const Expr&, const Expr&);
  Mul& make_mul(const Int&, const Expr&);
  Neg& make_neg(const Expr&);
  Pos& make_pos(const Expr&);

  // Relational expressions
  Eq& make_eq(const Expr&, const Expr&);
  Ne& make_ne(const Expr&, const Expr&);
  Lt& make_lt(const Expr&, const Expr&);
  Gt& make_gt(const Expr&, const Expr&);
  Le& make_le(const Expr&, const Expr&);
  Ge& make_ge(const Expr&, const Expr&);

  // Logical expressions
  And& make_and(const Expr&, const Expr&);
  Or& make_or(const Expr&, const Expr&);
  Imp& make_imp(const Expr&, const Expr&);
  Iff& make_iff(const Expr&, const Expr&);
  Not& make_not(const Expr&);

  // Binding
  Bind& make_bind(const Id&, const Type&);

  // Quantifiers
  Exists& make_exists(const Bind&, const Expr&);
  Forall& make_forall(const Bind&, const Expr&);

  // Types (for consistency)
  Bool_type& make_bool_type();
  Int_type& make_int_type();
  Kind_type& make_kind_type();

  // Expression factories
  Basic_factory<Id> ids;
  Basic_factory<Bool> bools;
  Basic_factory<Int> ints;
  Basic_factory<Var> vars;
  Basic_factory<Add> adds;
  Basic_factory<Sub> subs;
  Basic_factory<Mul> muls;
  Basic_factory<Neg> negs;
  Basic_factory<Pos> poss;
  Basic_factory<Eq> eqs;
  Basic_factory<Ne> nes;
  Basic_factory<Lt> lts;
  Basic_factory<Gt> gts;
  Basic_factory<Le> les;
  Basic_factory<Ge> ges;
  Basic_factory<And> ands;
  Basic_factory<Or> ors;
  Basic_factory<Imp> imps;
  Basic_factory<Iff> iffs;
  Basic_factory<Not> nots;
  Basic_factory<Bind> binds;
  Basic_factory<Exists> exs;
  Basic_factory<Forall> fas;

  // We don't need factories for these.
  Bool_type bool_type;
  Int_type int_type;
  Kind_type kind_type;
};

// -------------------------------------------------------------------------- //
// Context

// The Context class provides facilities for constructing well-formed 
// programs. 
//
// TODO: This may be more appropriately called a program construction 
// context. We should have other contexts: elaboration context,
// evaluation context, etc.
struct Context : Stack, Expr::Factory {
  Context();
  ~Context();

  // Type references
  const Bool_type& bool_type;
  const Int_type&  int_type;
  const Kind_type& kind_type;

  // Type definitions
  const Def* bool_def;
  const Def* int_def;

  Environment top;
};

} // namespace sarah

#endif
