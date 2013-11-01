
#include <iostream>

#include <syntax/Lexer.hpp>
#include <syntax/Parser.hpp>
#include <syntax/Sexpr.hpp>

#include <semantics/Language.hpp>
#include <semantics/Debug.hpp>

using namespace std;
using namespace sarah;

int 
main() {
  File f(cin);
  
  // Get the list of tokens.
  Lexer lex(f);
  Token_list toks = lex();

  // Parse the token stream
  Parser parser(toks);
  const Tree* ast = parser();
  if (ast)
    cout << sexpr(*ast) << '\n';
  else
    cout << "invalid program\n";
}
