
#include <iostream>

#include <syntax/Lexer.hpp>
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
  for(auto t : toks)
      cout << t << '\n';
}
