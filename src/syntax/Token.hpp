#ifndef SARAH_TOKEN_H
#define SARAH_TOKEN_H

#include <string>
#include <vector>
#include <iosfwd>

#include "utility/String.hpp"
#include "utility/Location.hpp"

namespace sarah {

/// Enumeration of toke types.
enum Token_type {
  // Non-language tokens
  Error_tok,
  
  // Punctuators
  Left_paren_tok,    // (
  Right_paren_tok,   // )
  Dot_tok,           // .
  Colon_tok,         // :

  // Tokens for arithmetic operators
  Plus_tok,          // +
  Minus_tok,         // -
  Star_tok,          // *

  // Tokens for relational operators
  Equal_equal_tok,   // ==
  Not_equal_tok,     // !=
  Less_tok,          // <
  Greater_tok,       // >
  Less_equal_tok,    // <=
  Greater_equal_tok, // >=

  // Tokens for logical operators
  And_tok,          // and
  Or_tok,           // or
  Not_tok,          // not
  Imp_tok,          // ->
  Iff_tok,          // <->

  // Keyword tokens
  True_tok,         // true
  False_tok,        // false
  Forall_tok,       // forall
  Exists_tok,       // Exists

  // Type keywords
  Bool_tok,         // bool
  Int_tok,          // int

  // Other terminals
  Int_literal_tok,  // [0-9]*
  Identifier_tok,   // [a-zA-Z_][a-zA-Z0-9_]
};

/// The Token class represents a token in the source language. Each token
/// is classified by its type and spelling.
///
/// Tokens are contextually convertible to bool, allowing them to be
/// tested for validity. For example:
///
///     if (Token t = consume(...))
///       // Executed whe t.type != Error_tok
struct Token {
  Token()
    : type(Error_tok)
  { }

  Token(Token_type t, Location l)
    : type(t), loc(l)
  { }
  
  Token(Token_type t, String s, Location l)
    : type(t), spell(s), loc(l)
  { }

  // Returns true for any non-error token.
  explicit operator bool() const { return type != Error_tok; }

  Token_type type;
  String     spell;
  Location   loc;
};

const char* spelling(Token_type);
const char* spelling(const Token&);

// Streamable
template<typename T, typename C>
  inline std::basic_ostream<T, C>&
  operator<<(std::basic_ostream<T, C>& os, const Token& t) {
    return os << t.spell;
  }


// Token list
using Token_list = std::vector<Token>;

// Token iterator
using Token_iterator = Token_list::const_iterator;

} // namespace sarah

#endif
