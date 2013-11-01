#ifndef SARAH_PARSER_HPP
#define SARAH_PARSER_HPP

#include "Token.hpp"
#include "Tree.hpp"

namespace sarah {

struct Tree;

// The parser class is responsible for transforming a sequence of
// tokens into an abstract syntax tree.
struct Parser : Tree::Factory {
  Parser(const Token_list& toks)
    : tokens(toks), current(tokens.begin())
  { }

  const Tree* operator()();

  const Token_list& tokens;
  Token_iterator    current;
};

} // namespace sarah

#endif
