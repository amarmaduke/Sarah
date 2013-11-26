
#include <iostream>

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
	vector<Elaboration*> expr_space;

	RuleSystem(vector<Elaboration*> axioms)
	: expr_space(axioms)
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
};


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
