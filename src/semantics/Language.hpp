
#ifndef SARAH_LANGUAGE_HPP
#define SARAH_LANGUAGE_HPP

#include <map>
#include <vector>

#include <utility/String.hpp>
#include <utility/Integer.hpp>

namespace sarah {

struct Expr;
struct Int;
struct Var;
struct Add;
struct Neg;
struct Mul;
struct Eq;
struct Ne;
struct Lt;
struct Gt;
struct Le;
struct Ge;
struct And;
struct Or;
struct Not;
struct Exists;
struct Forall;

// Types
struct Type;
struct Bool_type;
struct Int_type;

// Misc.
struct Top;
struct Context;

//
// Expressions
//

// The Expr class is the base class of all expressions in the abstract
// language.
struct Expr {
  struct Visitor;
  struct Factory;

  virtual ~Expr() { }

  virtual void accept(Visitor&) const = 0;
};

// A helper class for expr implementations. The B parameter indicates
// the direct base of the implementing class, and D is the derived class.
template<typename D, typename B = Expr>
  struct Expr_impl : B {
    template<typename... Args>
      Expr_impl(Args&&... args)
        : B(std::forward<Args>(args)...) { }


    virtual void accept(typename B::Visitor& v) const;
  };


// -------------------------------------------------------------------------- //
// Structure

// Atomic structures in the language.
template<typename T>
  struct Atom {
    Atom(const T& x)
      : data(x) { }

    const T& first() const { return data; }

    T data;
  };

// Non-atomic structures in the language.
template<typename... Args>
  struct Structure;

template<typename T>
  struct Structure<T> : std::tuple<const T*> {
    Structure(const T& t)
      : std::tuple<const T*>(&t) { }

    const T& first() const { return *std::get<0>(*this); }
  };

template<typename T1, typename T2>
  struct Structure<T1, T2> : std::tuple<const T1*, const T2*> {
    Structure(const T1& t1, const T2& t2)
      : std::tuple<const T1*, const T2*>(&t1, &t2) { }

    const T1& first() const { return *std::get<0>(*this); }
    const T2& second() const { return *std::get<1>(*this); }
  };

template<typename T1, typename T2, typename T3>
  struct Structure<T1, T2, T3> : std::tuple<const T1*, const T2*, const T3*> {
    Structure(const T1& t1, const T2& t2, const T3& t3)
      : std::tuple<const T1*, const T2*, const T3*>(&t1, &t2, &t3) { }

    const T1* first() const { return *std::get<0>(*this); }
    const T2* second() const { return *std::get<1>(*this); }
    const T3* third() const { return *std::get<2>(*this); }
  };


// -------------------------------------------------------------------------- //
// Expresssions

// An integer literal. 
struct Int : Atom<Integer>, Expr_impl<Int> {
  Int(Integer n)
    : Atom<Integer>(n) { }
  
  const Integer& value() const { return first(); }
};

// A variable name.
struct Var : Atom<String>, Expr_impl<Var> {
  Var(String s)
    : Atom<String>(s) { }

  String name() const { return first(); }
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


// Addition
struct Add : Binary_impl<Add> {
  Add(const Expr& l, const Expr& r)
    : Binary_impl<Add>(l, r) { }
};

// Arithmetic negation
struct Neg : Unary_impl<Neg> {
  Neg(const Expr& e)
    : Unary_impl<Neg>(e) { }
};

// Repeated addition. 
struct Mul : Structure<Int, Expr>, Expr_impl<Mul> {
  Mul(const Int& n, const Expr& e)
    : Structure<Int, Expr>(n, e) { }
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

// Logical not.
struct Not : Unary_impl<Not> {
  Not(const Expr& e)
    : Unary_impl<Not>(e) { }
};

// There exits.
struct Exists : Structure<Var, Expr>, Expr_impl<Exists> {
  Exists(const Var& n, const Expr& e)
    : Structure<Var, Expr>(n, e) { }
};

// Forall.
struct Forall : Structure<Var, Expr>, Expr_impl<Forall> {
  Forall(const Var& n, const Expr& e)
    : Structure<Var, Expr>(n, e) { }
};


// -------------------------------------------------------------------------- //
// Types

struct Type : Expr { };

template<typename D>
  using Type_impl = Expr_impl<D, Type>;

// The bool type.
struct Bool_type : Type_impl<Bool_type> { };

// The default int type.
struct Int_type : Type_impl<Int_type> { };


// -------------------------------------------------------------------------- //
// Visitor

struct Expr::Visitor {
  virtual void visit_expr(const Expr&);
  virtual void visit(const Int&);
  virtual void visit(const Var&);
  virtual void visit(const Add&);
  virtual void visit(const Neg&);
  virtual void visit(const Mul&);
  virtual void visit(const Eq&);
  virtual void visit(const Ne&);
  virtual void visit(const Lt&);
  virtual void visit(const Gt&);
  virtual void visit(const Le&);
  virtual void visit(const Ge&);
  virtual void visit(const And&);
  virtual void visit(const Or&);
  virtual void visit(const Not&);
  virtual void visit(const Exists&);
  virtual void visit(const Forall&);

  virtual void visit_type(const Type&);
  virtual void visit(const Bool_type&);
  virtual void visit(const Int_type&);
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
  Int& make_int(Integer);
  Var& make_var(String);

  // Arithmetic expressions.
  Add& make_add(const Expr&, const Expr&);
  Neg& make_neg(const Expr&);
  Mul& make_mul(const Int&, const Expr&);

  // Relational expressions
  Eq& make_eq(const Expr&, const Expr&);
  Ne& make_ne(const Expr&, const Expr&);
  Lt& make_lt(const Expr&, const Expr&);
  Gt& make_gt(const Expr&, const Expr&);
  Le& make_le(const Expr&, const Expr&);
  Ge& make_ge(const Expr&, const Expr&);

  // Logical expresssions
  And& make_and(const Expr&, const Expr&);
  Or& make_or(const Expr&, const Expr&);
  Not& make_not(const Expr&);

  // Quantifiers
  Exists& make_exists(const Var&, const Expr&);
  Forall& make_forall(const Var&, const Expr&);

  // Types
  Bool_type& make_bool_type();
  Int_type& make_int_type();

  Basic_factory<Int> ints;
  Basic_factory<Var> vars;
  Basic_factory<Add> adds;
  Basic_factory<Neg> negs;
  Basic_factory<Mul> muls;
  Basic_factory<Eq> eqs;
  Basic_factory<Ne> nes;
  Basic_factory<Lt> lts;
  Basic_factory<Gt> gts;
  Basic_factory<Le> les;
  Basic_factory<Ge> ges;
  Basic_factory<And> ands;
  Basic_factory<Or> ors;
  Basic_factory<Not> nots;
  Basic_factory<Exists> exs;
  Basic_factory<Forall> fas;

  Singleton_factory<Bool_type> bool_type;
  Singleton_factory<Int_type> int_type;

};

// -------------------------------------------------------------------------- //
// Context

// The Context class provides facilities for constructing well-formed 
// programs. 
//
// TODO: This may be more appropriately called a program construction 
// context. We should have other contexts: elaboration context,
// evaluation context, etc.
struct Context : Expr::Factory {
  Context();
  ~Context();

  const Bool_type&   bool_type;
  const Int_type&    int_type;
};

} // namespace sarah

#endif
