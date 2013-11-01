#ifndef SARAH_LEXER_HPP
#define SARAH_LEXER_HPP

#include <vector>

#include "Token.hpp"

namespace sarah {

// Token list
using Token_list = std::vector<Token>;

// Token iterator
using Token_iterator = Token_list::const_iterator;

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
