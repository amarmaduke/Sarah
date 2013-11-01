#ifndef STEVE_PARSER_HPP
#define STEVE_PARSER_HPP

#include "Token.hpp"
#include "Ast.hpp"

namespace steve {

// The parser class is responsible for transforming a sequence of
// tokens into an abstract syntax tree.
struct Parser : Tree_factory {
  explicit Parser(const Token_list& toks)
    : tokens(toks), current(tokens.begin())
  { }

  const Tree* operator()();

  const Token_list& tokens;
  Token_iterator    current;
};

} // namespace steve

#endif
