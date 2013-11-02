
#include <cctype>
#include <iostream>
#include <unordered_map>

#include "utility/Diagnostics.hpp"
#include "Lexer.hpp"

namespace sarah {

// -------------------------------------------------------------------------- //
// Classifiers

inline bool
is_identifier_0(char c) {
  return std::isalpha(c) or c == '_';
}

inline bool
is_identifier_N(char c) {
  return std::isalpha(c) or std::isdigit(c) or c == '_';
}

inline bool
is_newline(char c) {
  return c == '\n';
}

inline bool
is_whitespace(char c) {
  return c == ' ' or c == '\t';
}

// -------------------------------------------------------------------------- //
// Scanners

// Scan horizontal whitespace characters.
std::size_t
space(Lexer& lex) {
  auto i = lex.head;
  while(i != lex.tail and is_whitespace(*i)) {
    ++i;
  }
  return i - lex.head;
}

// Scan newline characters.
//
// TODO: Scan for newline sequences ('\r\n').
std::size_t
newline(Lexer& t) {
  if (t.head != t.tail and is_newline(*t.head))
    return 1;
  else
    return 0;
}

// Scan a comment.
std::size_t
comment(Lexer& lex) {
  auto i = lex.head;
  if (i == lex.tail or *i != '/')
    return 0;
  if (++i == lex.tail or *i != '/')
    return 0;
  while (++i != lex.tail and *i != '\n')
    ;
  return i - lex.head;
}

// Scan an integer literal.
std::size_t
int_literal(Lexer& lex) {
  auto i = lex.head;
  while(i != lex.tail and std::isdigit(*i))
    ++i;
  return i - lex.head;
}

// Scan an identifier.
std::size_t
identifier(Lexer& lex) {
  auto i = lex.head;
  if (i != lex.tail and is_identifier_0(*i))
    ++i;
  while (i != lex.tail and is_identifier_N(*i))
    ++i;
  return i - lex.head;
}

// Scan a symbol matching the given string.
std::size_t
symbol(Lexer& lex, const char* sym) {
  auto i = lex.head;
  auto p = sym;
  while (i != lex.tail and *p and *i == *p) {   // ? - what is *p checking .. null terminator?
    ++i;
    ++p;
  }
  return !*p ? i - lex.head : 0;
}

// -------------------------------------------------------------------------- //
// Keywords 

// Define a keyword table.
using Keyword_table = std::unordered_map<String, Token_type> ;
static Keyword_table keywords;

void
init_keywords() {
  Keyword_table t {
    // Operator keywords
    {"and", And_tok},
    {"or", Or_tok},
    {"not", Not_tok},
    {"true", True_tok},
    {"false", False_tok},
    {"forall", Forall_tok},
    {"exists", Exists_tok},

    // Type keywords
    {"bool", Bool_tok},
    {"int", Int_tok},
  };
  keywords.swap(t);
}

// Return the token type of the keyword str. If str is not a keyword,
// then it must be an identifier.
Token_type
lookup_keyword(String str) {
  // If the keyword table is empty, then populate it on the first pass.
  if (keywords.empty())
    init_keywords();

  // Find the given keyword.
  auto i = keywords.find(str);
  if (i != keywords.end())
    return i->second;
  else
    return Identifier_tok;
}

// Returns true if str is a keyword.
bool
is_keyword(String str) {
  if (lookup_keyword(str) != Identifier_tok)
    return true;
  else
    return false;
}

// Returns true if the next n characters denote a keyword.
bool
is_keyword(const Lexer& lex, std::size_t n) {
  String str(String(&*lex.head, n));
  return is_keyword(str);
}

// -------------------------------------------------------------------------- //
// Tokenization

// Increment the current line count and reset the current column
// to the beginning of the line.
inline void
feed(Lexer& lex) {
  lex.loc.line += 1;
  lex.loc.column = 1;
}

// Consume n characters from the token stream.
inline void
consume(Lexer& lex, std::size_t n) {
  lex.loc.column += n;
  lex.head += n;
}

// Consume a newline character from the token stream and
// update the current location accordingly.
inline void
consume_newline(Lexer& lex, std::size_t n) {
  consume(lex, n);
  feed(lex);
}

// Consume from this token until the end of the line or file,
// whichever comes first. Do not consume the end of line.
inline void
consume_comment(Lexer& lex) {
  while (lex.head != lex.tail and *lex.head != '\n')
    ++lex.head;
}

// Save an n-character token of the specified type.
inline void
save_token(Lexer& lex, Token_type t, std::size_t n) {
  lex.tokens.emplace_back(t, String(&*lex.head, n), lex.loc);
  consume(lex, n);
}

// Save a 1-character token of the specified type.
inline void
save_unigraph(Lexer& lex, Token_type t) {
  save_token(lex, t, 1);
}

// Save a 2-character token of the specified type.
inline void
save_digraph(Lexer& lex, Token_type t) {
  save_token(lex, t, 2);
}

inline void
save_trigraph(Lexer& lex, Token_type t) {
  save_token(lex, t, 3);
}

// Save a quoted sequence of characters. Save the enclosing
// quotes as part of the token.
inline void
save_quoted(Lexer& lex, Token_type t, std::size_t n) {
  lex.tokens.emplace_back(t, String(&*lex.head, n), lex.loc);
  consume(lex, n);
}

// Save an identifier.
inline void
save_identifier(Lexer& lex, std::size_t n)
{
  String str(String(&*lex.head, n));
  Token_type t = lookup_keyword(str);
  lex.tokens.emplace_back(t, str, lex.loc);
  consume(lex, n);
}

// Emit a diagnostic about an invalid character.
inline void
invalid_char(Lexer& lex) {
  error(lex.loc) << "invalid character '" << *lex.head << "'\n";
  consume(lex, 1);
}

void
tokenize(Lexer& lex) {
  while (lex.head != lex.tail) {
    switch (*lex.head) {
    case ' ':
    case '\t':
      if (std::size_t n = space(lex))
        consume(lex, n);
      break;
    
    case '\n':
      if (std::size_t n = newline(lex))
        consume_newline(lex, n);
      break;
    
    case '(':
      save_unigraph(lex, Left_paren_tok);
      break;

    case ')':
      save_unigraph(lex, Right_paren_tok);
      break;

    case ':':
      save_unigraph(lex, Colon_tok);
      break;

    case '.':
      save_unigraph(lex, Dot_tok);
      break;

    case '=':
      if (symbol(lex, "=="))
        save_digraph(lex, Equal_equal_tok);
      else
        invalid_char(lex);
      break;

    case '!':
      if (symbol(lex, "!="))
        save_digraph(lex, Not_equal_tok); 
      else
        invalid_char(lex);
      break;

    case '+':
      save_unigraph(lex, Plus_tok);
      break;

    case '-':
      if (symbol(lex, "->"))
        save_digraph(lex, Imp_tok);
      else
        save_unigraph(lex, Minus_tok);
      break;

    case '<':
      if (symbol(lex, "<->"))
        save_trigraph(lex, Iff_tok);
      else if (symbol(lex, "<="))
        save_digraph(lex, Less_equal_tok);
      else
        save_unigraph(lex, Less_tok);
      break;

    case '>':
      if (symbol(lex, ">="))
        save_digraph(lex, Greater_equal_tok);
      else
        save_unigraph(lex, Greater_tok);
      break;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      std::size_t n = int_literal(lex);
      save_token(lex, Int_literal_tok, n);
      break;
    }

    default:
      // Check for keywords and identifiers.
      if (std::isalpha(*lex.head)) {
        if (std::size_t n = identifier(lex)) {
          save_identifier(lex, n);
          break;
        }
      }
      else {
        invalid_char(lex);
      }

      break;
    }
  }
}

const Token_list&
Lexer::operator()() {
  tokenize(*this);
  return tokens;
}


} // namespace sarah
