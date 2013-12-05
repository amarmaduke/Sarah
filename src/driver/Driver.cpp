
#include <iostream>
#include <fstream>

#include <syntax/Lexer.hpp>
#include <syntax/Parser.hpp>
#include <syntax/Sexpr.hpp>

#include <semantics/Language.hpp>
#include <semantics/Elaborator.hpp>
#include <semantics/Translator.hpp>
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
  Elaborator* language;

  RuleSystem() {
    language = new Elaborator();
  }

  RuleSystem(Elaborator* axioms)
  : language(axioms)
  { }

  // Tries every implication rule to attempt to infer the antecedent.
  // Returns true if and only if new statements where added to the language.
  bool expand()
  {
    int derived_count = 0;
    vector<Elaboration>& elabs = language->elaborations;
    for(unsigned int i = 0; i < elabs.size(); ++i)
    {
      const Expr *expr = &elabs[i].expr();
      // Find an expression with root ->
      if(is<Imp,Expr>(expr))
      {
        // Dynamically Cast, then grab lef
        const Imp *imp = as<Imp,Expr>(expr);
        const Expr *left_expr = &imp->left();

        // Check every other expr in the language against left_expr
        bool found = false;
        for(unsigned int j = 0; j < elabs.size(); ++j)
        {
          const Expr *check = &elabs[j].expr();
          if(same(*check,*left_expr))
          {
            found = true;
            break;
          }
        }

        //if there is no identical match to the left expr, then look to see if
        //it  has AND or OR statmenets and evaluate them.
        if (found == false)
          found = and_or(*left_expr)

        // If we found an expr identical ("same") to left_expr then add
        // right_expr to the language.
        if (found)
        {
          Elaboration e(imp->right(),elabs[i].type());
          elabs.push_back(e);
          ++derived_count;
        }
      }
    }
    return derived_count > 0 ? true : false;
  }

  //function which handles AND and OR logic
  bool and_or(&Expr expr)
  {
    //check if expression is an OR
    if(is<Or, Expr>(expr))
    {
      const Or *or = as<Or, Expr>(expr);
      const Expr *left_expr = &or->left();
      const Expr *right_expr = &or->right();

      if ((and_or(left_expr) == true) || (and_or(right_expr) == true))
        return true;
      else
        return false;
    }
    //check if expression is an AND
    else if (is<And, Expr>(expr))
    {
      const And *and = as<And, Expr>(expr);
      const Expr *left_expr = &and->left();
      const Expr *right_expr = &and->right();

      if ((and_or(left_expr) == true) && (and_or(right_expr) == true))
        return true;
      else
        return false;
    }
    else
    {
      //otherwise it is a leaf node, check against all others in language
      vector<Elaboration>& elabs = language->elaborations;
      for (unsigned int i = 0; i < elabs.size(); ++i)
      {
        const Expr *check = &elabs[i].expr();
        if(same(*check, *left_expr))
          return true;
      }

    }

    //if all else fails, return false
    return false;
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
    for(unsigned int i = 0; i < language->elaborations.size(); ++i)
    {
      cout << language->elaborations[i].expr() << endl;
    }
  }
};

void elaborate_string(Elaborator* language, string input)
{
  Lexer lex(input);
  Token_list toks = lex();

  Parser parser(toks);
  const Tree* ast = parser();
  if (not ast) {
    cout << "invalid syntax\n";
  }

  const Elaboration e = language->elaborate(*ast);
  if (not e) {
    cout << "ill-formed program\n";
  }
}

int rule_system() {

  ifstream f("axioms");

  //vector<Elaboration> elaborations;
  RuleSystem rs;

  while(!f.eof()) {
    string input;
    getline(f,input);

    if(input.find_first_not_of(' ') == string::npos)
      continue;

    elaborate_string(rs.language,input);
  }

  rs.expand();
  rs.print();

  return 0;
}

int translate() {
  string in("forall x:int. 2 / x");
  Lexer lex(in);
  Token_list toks = lex();

  Parser parser(toks);
  const Tree* ast = parser();
  if (not ast) {
    cout << "invalid syntax\n";
  }

  Elaborator elab;
  Elaboration e = elab(*ast);
  if (not e) {
    cout << "ill-formed program\n";
  }

  Translator tra(elab);
  Elaboration e2 = tra(e.expr());
  return 0;
}


int
main() {
  //rule_system();
  translate();
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
