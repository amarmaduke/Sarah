
#include <iostream>
#include <fstream>

#include <syntax/Lexer.hpp>
#include <syntax/Parser.hpp>
#include <syntax/Sexpr.hpp>

#include <semantics/Language.hpp>
#include <semantics/Elaborator.hpp>
#include <semantics/Debug.hpp>

using namespace std;
using namespace sarah;


/*
	Currently the RuleSystem will only attempt a very limited form of modus
  ponens. That is, given an expr of the form (Expr A) -> (Expr B) in the
  expr_space with Expr A also in the expr_space, the system will infer (Expr B).
  It's important to note that in the implication "->" must be the root node.

  This is an unnecessary restriction on the normal modus ponens inference, but
  to start it off those are the assumptions made.
*/
struct RuleSystem {
  vector<Elaboration> language;

  RuleSystem(vector<Elaboration> axioms)
  : language(axioms)
  {}

  // Tries every implication rule to attempt to infer the antecedent.
  bool expand()
  {
    return true;
  }

  // Will expand until Expr e is found or the system has reached all inferable
  // expressions.
  bool search(Elaboration& e)
  {
    return true;
  }

  void print()
  {
    for(unsigned int i = 0; i < language.size(); ++i)
    {
      cout << language[i].expr() << endl;
    }
  }
};

Elaboration elaborate_string(string input)
{
  Lexer lex(input);
  Token_list toks = lex();

  Parser parser(toks);
  const Tree* ast = parser();
  if (not ast) {
    cout << "invalid syntax\n";
  }
  cout << "syntax: " << sexpr(*ast) << '\n';

  Elaborator elab;
  Elaboration prog = elab(*ast);
  if (not prog) {
    cout << "ill-formed program\n";
  }
  cout << "abstract: " << prog.expr() << '\n';

  return prog;
}

int rule_system() {

  ifstream f("axioms");

  vector<Elaboration> elaborations;
  vector<Elaborator> elabs;

  while(!f.eof()) {
    string input;
    getline(f,input);

    if(input.find_first_not_of(' ') == string::npos)
      continue;

    elaborations.push_back(elaborate_string(input));
  }

  cout << elaborations[0].expr() << '\n';

  return 0;
}


int
main() {
  rule_system();
  return 0;
  /*
  File f(cin);

  // Get the list of tokens.
  Lexer lex(test);
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
  //*/
}
