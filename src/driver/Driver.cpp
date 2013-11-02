
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
  if (not ast) {
    cout << "invalid syntax\n";
    return -1;
  }
  cout << "syntax: " << sexpr(*ast) << '\n';

  Elaborator elab;
  Elaboration prog = elab(*ast);
  if (not prog) {
    cout << "ill-formed program\n";
    return -1;
  }
  cout << "abstract: " << prog.expr() << '\n';
}
