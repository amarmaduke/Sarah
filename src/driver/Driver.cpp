
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
  vector<Elaborator*> language;

  RuleSystem(vector<Elaborator*> axioms)
  : language(axioms)
  {}

  // Tries every implication rule to attempt to infer the antecedent.
  // Returns true if and only if new statements where added to the language.
  bool expand()
  {
    int derived_count = 0;
    for(unsigned int i = 0; i < language.size(); ++i)
    {
      const Expr *expr = &language[i]->elaboration.expr();
      // Find an expression with root ->
      if(is<Imp,Expr>(expr))
      {
        // Dynamically Cast, then grab lef
        const Imp *imp = as<Imp,Expr>(expr);
        const Expr *left_expr = &imp->left();

        // Check every other expr in the language against left_expr
        bool found = false;
        for(unsigned int j = 0; j < language.size(); ++j)
        {
          const Expr *check = &language[j]->elaboration.expr();
          if(same(*check,*left_expr))
          {
            found = true;
            break;
          }
        }

        // If we found an expr identical ("same") to left_expr then add
        // right_expr to the language.
        if (found)
        {
          Elaboration e(imp->right(),language[i]->elaboration.type());
          Elaborator *add = new Elaborator(*language[i],e);
          language.push_back(add);
          ++derived_count;
        }
      }
    }
    return derived_count > 0 ? true : false;
  }

  // Will expand until Expr e is found or the system has reached all inferable
  // expressions.
  // TODO: Implement
  bool search(Elaboration& e)
  {
    return true;
  }

  void print()
  {
    for(unsigned int i = 0; i < language.size(); ++i)
    {
      cout << language[i]->elaboration.expr() << endl;
    }
  }
};

Elaborator* elaborate_string(string input)
{
  Lexer lex(input);
  Token_list toks = lex();

  Parser parser(toks);
  const Tree* ast = parser();
  if (not ast) {
    cout << "invalid syntax\n";
  }

  Elaborator* elab =  new Elaborator(*ast);
  const Elaboration& prog = elab->elaboration;
  if (not prog) {
    cout << "ill-formed program\n";
  }

  return elab;
}

int rule_system() {

  ifstream f("axioms");

  //vector<Elaboration> elaborations;
  vector<Elaborator*> elabs;

  while(!f.eof()) {
    string input;
    getline(f,input);

    if(input.find_first_not_of(' ') == string::npos)
      continue;

    elabs.push_back(elaborate_string(input));
  }

  RuleSystem rs(elabs);

  rs.expand();
  rs.print();

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
