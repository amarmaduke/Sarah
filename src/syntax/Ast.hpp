#ifndef STEVE_AST_HPP
#define STEVE_AST_HPP

#include <list>
#include <tuple>
#include <iosfwd>

#include "utility/Memory.hpp"

#include "Token.hpp"

namespace steve {

// -------------------------------------------------------------------------- //
// Trees

// The Tree class is the base of all nodes in the parse tree.
struct Tree { 
  struct Visitor;

  virtual ~Tree() { }

  virtual void accept(Visitor& v) const = 0;
};


// Provides a default implementation of common facilities in the
// AST Tree class.
template<typename Derived>
  struct Tree_impl : Tree
  {
    virtual void accept(Visitor& v) const;
  };

// The Tree_tuple class implements storage and accessors for the fields
// of a Tree_struct (see below). Specific accessors are defined by
// specializations of this class.
template<typename... Args>
  struct Tree_tuple;

template<typename T>
  struct Tree_tuple<T> : std::tuple<const T*> {
    Tree_tuple(const T* t)
      : std::tuple<const T*>(t) { }

    const T* first() const { return std::get<0>(*this); }
  };

template<typename T1, typename T2>
  struct Tree_tuple<T1, T2> : std::tuple<const T1*, const T2*> {
    Tree_tuple(const T1* t1, const T2* t2)
      : std::tuple<const T1*, const T2*>(t1, t2) { }

    const T1* first() const { return std::get<0>(*this); }
    const T2* second() const { return std::get<1>(*this); }
  };

template<typename T1, typename T2, typename T3>
  struct Tree_tuple<T1, T2, T3> : std::tuple<const T1*, const T2*, const T3*> {
    Tree_tuple(const T1* t1, const T2* t2, const T3* t3)
      : std::tuple<const T1*, const T2*, const T3*>(t1, t2, t3) { }

    const T1* first() const { return std::get<0>(*this); }
    const T2* second() const { return std::get<1>(*this); }
    const T3* third() const { return std::get<2>(*this); }
  };

// The Tree_struct class is used to define the structure of nodes
// in the syntax tree. It is simply a tuple of pointers to trees
// and tokens.
template<typename Derived, typename... Args>
  struct Tree_struct : Tree_impl<Derived>, Tree_tuple<Args...> {
    using  Base = Tree_tuple<Args...>;

    template<typename T1>
      Tree_struct(const T1* t1) 
        : Base(t1) { }

    template<typename T1, typename T2>
      Tree_struct(const T1* t1, const T2* t2) 
        : Base(t1, t2) { }

    template<typename T1, typename T2, typename T3>
      Tree_struct(const T1* t1, const T2* t2, const T3* t3)
        : Base(t1, t2, t3) { }
  };

/// The Tree_atom class provides a definition for all tree nodes
/// that contain only a single token.
template<typename Derived>
  struct Tree_atom : Tree_struct<Derived, Token> {
    using Base = Tree_struct<Derived, Token>;
    
    Tree_atom(const Token* k)
      : Base(k) { }

    /// Return the token associated with the syntactic atom.
    const Token* token() const { return this->first(); }

    /// Return the lexeme associated with the syntactic atom.
    String spelling() const { return token()->spell; }
  };

/// The Tree_sequence class defines a sequence of objects. Here, T
/// is either a Token or a type derived from Tree.
template<typename T = Tree>
  struct Tree_sequence : std::vector<const T*> {
    Tree_sequence() = default;
  };


/// An enclosed tree is a sub-tree enclosed by a pair of tokens.
struct Enclosed_tree : Tree_struct<Enclosed_tree, Token, Token, Tree> {
  using Base = Tree_struct<Enclosed_tree, Token, Token, Tree>;

  Enclosed_tree(const Token* o, const Token* c, const Tree* a)
    : Base(o, c, a) { }

  const Token* open() const { return first(); }
  const Token* close() const { return second(); }
  const Tree* arg() const { return third(); }
};

/// A sequence of trees.
struct Sequence_tree : Tree_impl<Sequence_tree>, Tree_sequence<> {
  Sequence_tree(Tree_sequence<>&& x) : Tree_sequence<>(std::move(x)) { }
};

/// Representation of literals in the program source.
struct Literal_tree : Tree_atom<Literal_tree> { 
  using Base = Tree_atom<Literal_tree>;

  Literal_tree(const Token* k)
    : Base(k) { }
};

/// Representation of identifiers in the program source.
struct Identifier_tree : Tree_atom<Identifier_tree> { 
  using Base = Tree_atom<Identifier_tree>;

  Identifier_tree(const Token* k)
    : Base(k) { }
};

/// A unary term comprised of a token denoting an operation and its
/// sole argument.
struct Unary_tree : Tree_struct<Unary_tree, Token, Tree> {
  using Base = Tree_struct<Unary_tree, Token, Tree>;

  Unary_tree(const Token* o, const Tree* a)
    : Base(o, a) { }

  const Token* op() const { return first(); }
  const Tree* arg() const { return second(); }
};

/// A binary tree is comprised of a token denoting an operation and its
/// left and right arguments.
struct Binary_tree : Tree_struct<Binary_tree, Token, Tree, Tree> {
  using Base = Tree_struct<Binary_tree, Token, Tree, Tree>;

  Binary_tree(const Token* o, const Tree* l, const Tree* r)
    : Base(o, l, r) { }
  
  const Token* op() const { return first(); }
  const Tree* left() const { return second(); }
  const Tree* right() const { return third(); }
};

/// Represents a call expression. This is a binary tree having a
/// call target (e.g., a function) and a sequence of arguments.
struct Call_tree : Tree_struct<Call_tree, Tree, Tree> {
  using Base = Tree_struct<Call_tree, Tree, Tree>;

  Call_tree(const Tree* t, const Tree* a)
    : Base(t, a) { }

  const Tree* target() const { return first(); }
  const Tree* args() const { return second(); }
};

/// Represents an access expression. This is a binary tree having
/// a scope and its accessed member.
struct Access_tree : Tree_struct<Access_tree, Tree, Tree> {
  using Base = Tree_struct<Access_tree, Tree, Tree>;

  Access_tree(const Tree* s, const Tree* m)
    : Base(s, m) { }

  const Tree* scope() const { return first(); }
  const Tree* member() const { return second(); }
};

/// Represents an index expression. The is a binary tree whose first
/// argument represents a store (something that can be indexed), and
/// whose second argument is an index.
struct Index_tree : Tree_struct<Index_tree, Tree, Tree> {
  using Base = Tree_struct<Index_tree, Tree, Tree>;

  Index_tree(const Tree* s, const Tree* k)
    : Base(s, k) { }

  const Tree* store() const { return first(); }
  const Tree* index() const { return second(); }
};

/// Representation of types in the program source. Note that this will
/// change to accommodate a broader range of types (record types, variant
/// types, type-ids, etc).
struct Type_tree : Tree_atom<Type_tree> { 
  using Base = Tree_atom<Type_tree>;

  Type_tree(const Token* t)
    : Base(t) { }
};

/// Object declarators.
struct Value_tree : Tree_struct<Value_tree, Tree> {
  using Base = Tree_struct<Value_tree, Tree>;

  Value_tree(const Tree* n)
    : Base(n) { }

  const Tree* name() const { return first(); }
};

/// Function declarators.
struct Function_tree : Tree_struct<Function_tree, Tree, Tree> {
  using Base = Tree_struct<Function_tree, Tree, Tree>;

  Function_tree(const Tree* n, const Tree* p)
    : Base(n, p) { }

  const Tree* name() const { return first(); }
  const Tree* params() const { return second(); }
};

/// Template declarators.
struct Template_tree : Tree_struct<Template_tree, Tree, Tree, Tree> {
  using Base = Tree_struct<Template_tree, Tree, Tree, Tree>;

  Template_tree(const Tree* n, const Tree* t, const Tree*f)
    : Base(n, t, f) { }

  const Tree* name() const { return first(); }
  const Tree* tparams() const { return second(); }
  const Tree* fparams() const { return third(); }
};

/// Representation of parameters in the program source.
struct Parameter_tree : Tree_struct<Parameter_tree, Tree, Tree> {
  using Base = Tree_struct<Parameter_tree, Tree, Tree>;

  Parameter_tree(const Tree* n, const Tree* t)
    : Base(n, t) { }
  
  const Tree* name() const { return first(); }
  const Tree* type() const { return second(); }
};

/// A definition statement is a triple comprising a declarator, type, and 
/// initializer.
struct Def_tree : Tree_struct<Def_tree, Tree, Tree, Tree> {
  using Base = Tree_struct<Def_tree, Tree, Tree, Tree>;

  Def_tree(const Tree* d, const Tree* t, const Tree* i)
    : Base(d, t, i) { }

  const Tree* decl() const { return first(); }
  const Tree* type() const { return second(); }
  const Tree* init() const { return third(); }
};

/// A scan statement is represented by the name and type of the
/// introduced input.
struct Scan_tree : Tree_struct<Scan_tree, Tree, Tree> {
  using Base = Tree_struct<Scan_tree, Tree, Tree>;

  Scan_tree(const Tree* n, const Tree* t)
    : Base(n, t) { }

  const Tree* name() const { return first(); }
  const Tree* type() const { return second(); }
};

/// A print statement contains its printed argument.
struct Print_tree : Tree_struct<Print_tree, Tree> {
  using Base = Tree_struct<Print_tree, Tree>;

  Print_tree(const Tree* t)
    : Base(t) { }

  const Tree* arg() const { return first(); }
};

struct Program_tree : Tree_impl<Program_tree>, Tree_sequence<> {
  Program_tree(Tree_sequence<>&& s) : Tree_sequence<>(std::move(s)) { }

  const Tree_sequence<>& stmts() const { return *this; }
};

struct Tree_factory
{
  const Enclosed_tree* make_enclosed(const Token*, const Token*, const Tree*);
  const Sequence_tree* make_sequence(Tree_sequence<>&&);

  const Identifier_tree* make_identifier(const Token*);
  const Literal_tree* make_literal(const Token*);
  const Unary_tree* make_unary(const Token*, const Tree*);
  const Binary_tree* make_binary(const Token*, const Tree*, const Tree*);
  const Call_tree* make_call(const Tree*, const Tree*);
  const Access_tree* make_access(const Tree*, const Tree*);
  const Index_tree* make_index(const Tree*, const Tree*);
  
  const Type_tree* make_type(const Token*);
  
  const Value_tree* make_value(const Tree*);
  const Function_tree* make_function(const Tree*, const Tree*);
  const Template_tree* make_template(const Tree*, const Tree*, const Tree*);
  const Parameter_tree* make_parameter(const Tree*, const Tree*);

  const Def_tree* make_def(const Tree*, const Tree*, const Tree*);
  const Scan_tree* make_scan(const Tree*, const Tree*);
  const Print_tree* make_print(const Tree*);

  const Program_tree* make_program(Tree_sequence<>&&);

  Basic_factory<Enclosed_tree> encs;
  Basic_factory<Sequence_tree> seqs;
  Basic_factory<Identifier_tree> ids;
  Basic_factory<Literal_tree> lits;  
  Basic_factory<Unary_tree> uns;
  Basic_factory<Binary_tree> bins;
  Basic_factory<Call_tree> calls;
  Basic_factory<Access_tree> accs;
  Basic_factory<Index_tree> idxs;
  Basic_factory<Type_tree> types;
  Basic_factory<Value_tree> vals;
  Basic_factory<Function_tree> fns;
  Basic_factory<Template_tree> tmpls;
  Basic_factory<Parameter_tree> params;
  Basic_factory<Def_tree> defs;
  Basic_factory<Scan_tree> scans;
  Basic_factory<Print_tree> prints;
  Basic_factory<Program_tree> progs;
};

// -------------------------------------------------------------------------- //
// Visitor

// Ast Visitor
struct Tree::Visitor {
  virtual void visit(const Enclosed_tree*) { }
  virtual void visit(const Sequence_tree*) { }
  virtual void visit(const Identifier_tree*) { }
  virtual void visit(const Value_tree*) { }
  virtual void visit(const Function_tree*) { }
  virtual void visit(const Template_tree*) { }
  virtual void visit(const Literal_tree*) { }
  virtual void visit(const Unary_tree*) { }
  virtual void visit(const Binary_tree*) { }
  virtual void visit(const Call_tree*) { }
  virtual void visit(const Access_tree*) { }
  virtual void visit(const Index_tree*) { }
  virtual void visit(const Type_tree*) { }
  virtual void visit(const Parameter_tree*) { }
  virtual void visit(const Def_tree*) { }
  virtual void visit(const Print_tree*) { }
  virtual void visit(const Scan_tree*) { }
  virtual void visit(const Program_tree*) { }
};

// -------------------------------------------------------------------------- //
// Implementation

template<typename Derived>
  inline void
  Tree_impl<Derived>::accept(Visitor& v) const {
    v.visit(static_cast<const Derived*>(this));
  }

} // namespace steve





#endif
