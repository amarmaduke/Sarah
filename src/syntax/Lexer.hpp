#ifndef SARAH_LEXER_HPP
#define SARAH_LEXER_HPP

#include <vector>

#include "Token.hpp"

namespace sarah {

// The Lexer is responsible for the tokenization of an input file.
struct Lexer {
  Lexer(const File& f)
    : loc(f), head(f.begin()), tail(f.end())
  { }

  const Token_list& operator()();

  Location loc;
  String::iterator head;
  String::iterator tail;
  Token_list tokens;
};


} // namespace sarah

#endif
