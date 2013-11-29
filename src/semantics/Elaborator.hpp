
#ifndef SARAH_ELABORATOR_HPP
#define SARAH_ELABORATOR_HPP

#include <utility>

#include "Language.hpp"

namespace sarah {

struct Tree;
struct Expr;
struct Type;

// An elaboration is a pair containing an expression and its type. Note
// that an elaboration can be empty, if the expression could not be
// translated or the type could not be inferred or fails to check.
struct Elaboration : std::pair<const Expr*, const Type*> {
  Elaboration()
    : std::pair<const Expr*, const Type*>() { }

  Elaboration(const Expr& e, const Type& t)
    : std::pair<const Expr*, const Type*>(&e, &t) { }

  const Expr& expr() const { return *this->first; }
  const Type& type() const { return *this->second; }

  // Returns true when then the expression is valid and
  // has a valid type.
  explicit operator bool() const { return this->first and this->second; }
};


// The Elaborator is responsible for transforming a parse tree into
// an abstract syntax tree.
//
// NOTE: The Elaborator is really a product of the binding the syntax to
// the abstract language. If we want a different front-end syntax, then
// we'd support multiple front-end elaborators. It is tempting to move this
// class into the syntax repository.
struct Elaborator : Context {

  Elaborator(const Tree& tree)
    : elaboration((*this)(tree)) { }

  Elaborator(Elaborator& c, Elaboration& e)
    : elaboration(e)
  {
    Context::top = c.Context::top;
  }

  Elaboration elaboration;

  Elaboration operator()(const Tree&);
  //void elaborate(const Tree&);
};

} // namespace sarah

#endif
