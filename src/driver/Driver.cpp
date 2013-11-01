
#include <iostream>

#include <syntax/Lexer.hpp>
#include <syntax/Parser.hpp>
#include <syntax/Sexpr.hpp>

#include <semantics/Language.hpp>
#include <semantics/Elaborator.hpp>
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
    cout << "syntax: " << sexpr(*ast) << '\n';
  else
    cout << "invalid syntax\n";

  Elaborator elab;
  Elaboration prog = elab(*ast);
  if (prog)
    cout << "abstract: " << prog.expr() << '\n';
  else
    cout << "ill-formed program\n";
}
