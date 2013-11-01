#ifndef SARAH_TREE_HPP
#define SARAH_TREE_HPP

#include <list>
#include <tuple>
#include <iosfwd>

#include "utility/Memory.hpp"
#include "utility/Structure.hpp"

#include "Token.hpp"

namespace sarah {

// -------------------------------------------------------------------------- //
// Trees

// The Tree class is the base of all nodes in the parse tree.
struct Tree { 
  struct Factory;
  struct Visitor;

  virtual ~Tree() { }

  virtual void accept(Visitor& v) const = 0;
};

// Provides a default implementation of common facilities in the
// AST Tree class.
template<typename Derived>
  struct Tree_impl : Tree {
    virtual void accept(Visitor& v) const;
  };


struct Enclosed_tree : Structure<Token, Token, Tree>, Tree_impl<Enclosed_tree> {
  Enclosed_tree(const Token& o, const Token& c, const Tree& a)
    : Structure<Token, Token, Tree>(o, c, a) { }

  const Token& open() const { return first(); }
  const Token& close() const { return second(); }
  const Tree& arg() const { return third(); }
};

// Represents literals.
struct Literal_tree : Structure<Token>, Tree_impl<Literal_tree> { 
  Literal_tree(const Token& k)
    : Structure<Token>(k) { }

  const Token& token() const { return first(); }
};

// Represents identifiers.
struct Identifier_tree : Structure<Token>, Tree_impl<Identifier_tree> { 
  Identifier_tree(const Token& k)
    : Structure<Token>(k) { }

  const Token& token() const { return first(); }
};

// Represents unary expressions.
struct Unary_tree : Structure<Token, Tree>, Tree_impl<Unary_tree> {
  Unary_tree(const Token& o, const Tree& a)
    : Structure<Token, Tree>(o, a) { }

  const Token& op() const { return first(); }
  const Tree& arg() const { return second(); }
};

// Represents binary operators and quantifiers.
//
// TODO: Represent quantifiers separately?
struct Binary_tree : Structure<Token, Tree, Tree>, Tree_impl<Binary_tree> {
  Binary_tree(const Token& o, const Tree& l, const Tree& r)
    : Structure<Token, Tree, Tree>(o, l, r) { }
  
  const Token& op() const { return first(); }
  const Tree& left() const { return second(); }
  const Tree& right() const { return third(); }
};


struct Tree::Factory {
  Enclosed_tree& make_enclosed(const Token&, const Token&, const Tree&);

  Literal_tree& make_literal(const Token&);
  Identifier_tree& make_identifier(const Token&);
  Unary_tree& make_unary(const Token&, const Tree&);
  Binary_tree& make_binary(const Token&, const Tree&, const Tree&);
  
  Basic_factory<Enclosed_tree> encs;
  Basic_factory<Literal_tree> lits;  
  Basic_factory<Identifier_tree> ids;
  Basic_factory<Unary_tree> uns;
  Basic_factory<Binary_tree> bins;
};

// -------------------------------------------------------------------------- //
// Visitor

// Ast Visitor
struct Tree::Visitor {
  virtual void visit(const Enclosed_tree&) { }
  virtual void visit(const Literal_tree&) { }
  virtual void visit(const Identifier_tree&) { }
  virtual void visit(const Unary_tree&) { }
  virtual void visit(const Binary_tree&) { }
};

// -------------------------------------------------------------------------- //
// Implementation

template<typename Derived>
  inline void
  Tree_impl<Derived>::accept(Visitor& v) const {
    v.visit(*static_cast<const Derived*>(this));
  }

} // namespace sarah





#endif
