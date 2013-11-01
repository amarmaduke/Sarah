
#include <iostream>

#include "utility/Diagnostics.hpp"
#include "Parser.hpp"

namespace sarah {

// Returns a pointer to the current token.
const Token*
peek(const Parser& p) { return &*p.current; }

// Returns a popinter to the nth token past the current token.
const Token*
lookahead(const Parser& p, std::size_t n) {
  if (p.tokens.end() - p.current > std::ptrdiff_t(n))
    return &*(p.current + n); 
  else
    return nullptr;
}

// Returns true if the next token has type t.
inline bool
next_token_is(const Parser& p, Token_type t) {
  return peek(p)->type == t; 
}

// Returns true if the nth token has type t.
inline bool
nth_token_is(const Parser& p, std::size_t n, Token_type t) { 
  if (const Token* tok = lookahead(p, n))
    return tok->type == t;
  else
    return false;
}

// Returns the current location in the program source.
const Location&
location(const Parser& p) { return peek(p)->loc; }

// Return a diagnostic stream indicating an error at the current 
// position in thje program source.
inline std::ostream&
error(const Parser& p) { return error(location(p)); }

// Returns the current token, and advances the parser.
inline const Token*
consume(Parser& p) {
  const Token* tok = &*p.current;
  ++p.current;
  return tok;
}

// If the current token is of type T, advance to the next token
// and return true. Otherwise, return false.
const Token*
accept(Parser& p, Token_type t) {
  if (peek(p)->type == t)
    return consume(p);
  else
    return nullptr;
}

// Require the current token to match t, consuming it. Generate a
// diagnostic if the current token does not match.
const Token*
expect(Parser& p, Token_type t) {
  if (const Token* tok = accept(p, t))
    return tok;
  error(p) << "expected '" << spelling(t) << "'\n";
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Parser combinators and facilities

using Symbol = const Token* (*)(Parser&);
using Production = const Tree* (*)(Parser&);

// This template binds the production functions to the spelling
// of that production. It is used for diagnosing parse errors.
template<Production P>
  struct production;

// Either parse the given production or emit an error diagnosting the
// expression that is expected.
template<Production Prod>
  const Tree*
  parse_expected(Parser& p) {
    if (const Tree* n = Prod(p))
      return n;
    else 
      return nullptr;
  }

// Parse a left-associative binary expression.
//
//    left(op, sub) ::= sub [op sub]*
template<Symbol Op, Production Sub>
  const Tree*
  parse_left(Parser& p) {
    if (const Tree* l = Sub(p)) {
      while (const Token* t = Op(p)) {
        if (const Tree* r = parse_expected<Sub>(p))
          l = &p.make_binary(*t, *l, *r);
        else
          return nullptr;
      }
      return l;
    }
    return nullptr;
  }

// Parse a right-associative binary expression.
//
//    right(op, self, sub) ::= sub [op this]
template<Symbol Op, Production Self, Production Sub>
  const Tree*
  parse_right(Parser& p) {
    if (const Tree* l = Sub(p)) {
      if (const Token* t = Op(p)) {
        if (const Tree* r = parse_expected<Self>(p))
          l = &p.make_binary(*t, *l, *r);
        else
          return nullptr;
      }
      return l;
    }
    return nullptr;
  }

// Parse a unary expression.
//
//   unary(op, self, sub) ::= sub | op self
template<Symbol Op, Production Self, Production Sub>
  const Tree*
  parse_unary(Parser& p) {
    if (const Token* t = Op(p)) {
      if (const Tree* n = parse_expected<Self>(p))
        return &p.make_unary(*t, *n);
      return nullptr;
    }
    return Sub(p);
  }


// The kind of enclosing.
//
// NOTE: This is designed for more enclosings.
enum Enclosing { Paren_enc };

static Token_type enclosing_tokens[][2] {
  {Left_paren_tok, Right_paren_tok}
};

inline Token_type
open_token(Enclosing e) { return enclosing_tokens[int(e)][0]; }

inline Token_type
close_token(Enclosing e) { return enclosing_tokens[int(e)][1]; }

template<Enclosing Enc, Production Prod>
  const Tree*
  parse_enclosed(Parser& p) {
    if (const Token* l = accept(p, open_token(Enc)))
      if (const Tree* n = parse_expected<Prod>(p))
        if (const Token* r = expect(p, close_token(Enc)))
          return &p.make_enclosed(*l, *r, *n);
    return nullptr;
  }

// Parse a paren-enclosed production.
template<Production Prod>
  inline const Tree*
  parse_paren_enclosed(Parser& p) {
    return parse_enclosed<Paren_enc, Prod>(p);
  }

// -------------------------------------------------------------------------- //
// Parser

const Tree* parse_identifier(Parser&);
const Tree* parse_boolean_lit(Parser&);
const Tree* parse_integer_lit(Parser&);

const Tree* parse_primary_expr(Parser&);
const Tree* parse_sign_expr(Parser&);
const Tree* parse_multiplicative_expr(Parser&);
const Tree* parse_additive_expr(Parser&);
const Tree* parse_ordering_expr(Parser&);
const Tree* parse_equality_expr(Parser&);
const Tree* parse_not_expr(Parser&);
const Tree* parse_and_expr(Parser&);
const Tree* parse_or_expr(Parser&);
const Tree* parse_implication_expr(Parser&);
const Tree* parse_iff_expr(Parser&);
const Tree* parse_expr(Parser&);


// Parse a boolean literal.
//
// boolean-lit ::= 'true' | 'false'
const Tree*
parse_boolean_lit(Parser& p) {
  if (const Token* t = accept(p, True_tok))
    return &p.make_literal(*t);
  if (const Token* t = accept(p, False_tok))
    return &p.make_literal(*t);
  return nullptr;
}

// Parse an integer literal.
//
// integer-lit ::= [0-9]*
const Tree*
parse_integer_lit(Parser& p) {
  if (const Token* t = accept(p, Int_literal_tok))
    return &p.make_literal(*t);
  return nullptr;
}

// Parse an identifier.
//
// identifier ::= [a-zA-Z_][a-zA-Z0-9_]*
const Tree*
parse_identifier(Parser& p) {
  if (const Token* t = accept(p, Identifier_tok))
    return &p.make_identifier(*t);
  return nullptr;
}

// Parse a primary expression.
//
// primary-expr ::= default
//                | boolean-lit 
//                | integer-lit
//                | identifier
//                | '(' expression ')'
const Tree*
parse_primary_expr(Parser& p) {
  if (const Tree* t = parse_boolean_lit(p))
    return t;
  if (const Tree* t = parse_integer_lit(p))
    return t;
  if (const Tree* t = parse_identifier(p))
    return t;
  if (const Tree* n = parse_enclosed<Paren_enc, parse_expr>(p))
    return n;
  return nullptr;
}

// Parse an arithmetic sign operator.
//
//     sign-op ::= '+' | '-'
inline const Token*
parse_sign_op(Parser& p) {
  switch(peek(p)->type) {
  case Plus_tok:
  case Minus_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

// Parse an arithmetic sign expression.
//
//     sign-expr ::= unary(sign-expr, primary-expr, sign-op)
const Tree*
parse_sign_expr(Parser& p) {
  return parse_unary<parse_sign_op, parse_sign_expr, parse_primary_expr>(p);
}

// Parse a multiplicative operator.
//
//     multiplicative-op ::= '*' | '/' | '%'
inline const Token*
parse_multiplicative_op(Parser& p) {
  if (peek(p)->type == Star_tok)
    return consume(p);
  else
    return nullptr;
}

// Parse a multiplicative expression.
//
//     multiplicative-expr ::= left(sign-expr, multplicative-op)
const Tree*
parse_multiplicative_expr(Parser& p) {
  return parse_left<parse_multiplicative_op, parse_sign_expr>(p);
}

// Parse and additive operator.
//
//     additive-op ::= '+' | '-'
inline const Token*
parse_additive_op(Parser& p) {
  switch(peek(p)->type) {
  case Plus_tok:
  case Minus_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

// Parse an additive expression.
//
//     additive-expr ::= left(multiplicative-expr, additive-op)
const Tree*
parse_additive_expr(Parser& p) {
  return parse_left<parse_additive_op, parse_multiplicative_expr>(p);
}

// Parsse an relational ordering operator.
//
//     ordering-op ::= '<' | '>' | '<=' | '>='
inline const Token*
parse_ordering_op(Parser& p) {
  switch(peek(p)->type) {
  case Less_tok:
  case Greater_tok:
  case Less_equal_tok:
  case Greater_equal_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

// Parse a relational ordering expression.
//
//     ordering-expr ::= left(ordering-op, additive-expr)
const Tree*
parse_ordering_expr(Parser& p) {
  return parse_left<parse_ordering_op, parse_additive_expr>(p);
}

// Parse a relational equality operator.
//
//     equality-op ::= '==' | '!='
const Token*
parse_equality_op(Parser& p) {
  switch(peek(p)->type) {
  case Equal_equal_tok:
  case Not_equal_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

// Parse a relational equality expression.
//     equality-expr ::= left(equality-op, ordering-expr)
const Tree*
parse_equality_expr(Parser& p) {
  return parse_left<parse_equality_op, parse_ordering_expr>(p);
}

// Parse a logical not operator.
//
//     not-op ::= 'not'
inline const Token*
parse_not_op(Parser& p) { return accept(p, Not_tok); }

// Parse a logical not expression.
//
//     not-expr ::= unary(not-op, equality-expr)
const Tree*
parse_not_expr(Parser& p) {
  return parse_unary<parse_not_op, 
                     parse_not_expr, 
                     parse_equality_expr>(p);
}

// Parse a logical and operator.
//
//     and-operator ::= 'and'
inline const Token*
parse_and_op(Parser& p) { return accept(p, And_tok); }

// Parse a logical and expression.
//
//     and-expression ::= left(and-op, not-expr)
const Tree*
parse_and_expr(Parser& p) {
  return parse_left<parse_and_op, parse_not_expr>(p);
}

// Parse a logical or operator
//
//     or-operator ::= 'or'
inline const Token*
parse_or_op(Parser& p) { return accept(p, Or_tok); }

// Parse a logical or expression.
//
//     or-expression ::= left(or-op, and-expr)
const Tree*
parse_or_expr(Parser& p) {
  return parse_left<parse_or_op, parse_and_expr>(p);
}

// Parse the implication operators
//
//    implication-op ::= '->'
const Token*
parse_implication_op(Parser& p) { return accept(p, Imp_tok); }

// Parse an implication expression.
//
//    implication-expr ::= right(implication-op, logical_or_expr)
const Tree*
parse_implication_expr(Parser& p) { 
  return parse_right<parse_implication_op, 
                     parse_implication_expr,
                     parse_or_expr>(p);
}

// Parse the if-and-only-if operator.
//
//    iff-op ::= '<->'
const Token*
parse_iff_op(Parser& p) { return accept(p, Iff_tok); }

// Parse an if-and-only-if expression.
//
//    iff-expr ::= left(iff_op, implication-expr)
const Tree*
parse_iff_expr(Parser& p) {
  return parse_left<parse_iff_op, parse_implication_expr>(p);
}

// Parse a quantifier.
//
//    quantifier ::= 'forall' | 'exists'
const Token*
parse_quanitifier(Parser& p) {
  const Token* tok = peek(p);
  if (tok->type == Forall_tok || tok->type == Exists_tok)
    return consume(p);
  else
    return nullptr;
}

// Parse a quantified expression.
//
//    quantified-expr ::= quantifier identifier '.' expr
const Tree*
parse_quantified_expr(Parser& p) {
  if (const Token* q = parse_quanitifier(p))
    if (const Tree* n = parse_identifier(p))
      if (expect(p, Dot_tok))
        if (const Tree* e = parse_expr(p))
          return &p.make_binary(*q, *n, *e);
  return nullptr;
}

// Parse an expr.
//
//     expr ::= quantified-expr | iff-expression
const Tree*
parse_expr(Parser& p) { 
  if (const Tree* t = parse_quantified_expr(p))
    return t;
  else
    return parse_iff_expr(p); 
}

// -------------------------------------------------------------------------- //
// Parser

const Tree*
Parser::operator()() {
  if (current != tokens.end())
    return parse_expected<parse_expr>(*this);
  else
    return nullptr;
}

} // namespace sarah
