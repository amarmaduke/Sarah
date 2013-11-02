
#include "Token.hpp"

namespace sarah {

// Returns the spelling of a token type. Returns nullptr if
// the spelling cannot be inferred by the type alone.
const char*
spelling(Token_type t)
{
  switch (t) {
  // Punctuators
  case Left_paren_tok: return "(";
  case Right_paren_tok: return ")";
  case Colon_tok: return ":";
  case Dot_tok: return ".";

  // Tokens for arithmetic operaitos
  case Plus_tok: return "+";
  case Minus_tok: return "-";
  case Star_tok: return "*";

  // Tokens for relational operators
  case Equal_equal_tok: return "==";
  case Not_equal_tok: return "!=";
  case Less_tok: return "<";
  case Greater_tok: return ">";
  case Less_equal_tok: return "<=";
  case Greater_equal_tok: return ">=";

  // Tokens for logical operators
  case And_tok: return "and";
  case Or_tok: return "or";
  case Not_tok: return "not";
  case Imp_tok: return "->";
  case Iff_tok: return "<->";

  // Value keywords
  case True_tok: return "true";
  case False_tok: return "false";
  case Forall_tok: return "forall";
  case Exists_tok: return "exists";

  // Type keywords
  case Bool_tok: return "bool";
  case Int_tok: return "int";

  // Everything else.
  default: return "<unknown-token>";
  }
}

// Return the spelling of the token.
const char*
spelling(const Token& t) { return t.spell.data(); }

} // namespace sarah
