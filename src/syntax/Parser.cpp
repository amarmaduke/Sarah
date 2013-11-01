
#include <iostream>

#include <typeinfo>

#include "Parser.hpp"
#include "Sexpr.hpp"
#include "Diagnostics.hpp"

namespace steve {

// Returns a pointer to the current token.
const Token*
peek(const Parser& p) { return &*p.current; }

// Returns a popinter to the nth token past the current token.
const Token*
lookahead(const Parser& p, std::size_t n) {
  if (p.tokens.end() - p.current > std::ptrdiff_t(n))
    return &*(p.current + n); 
  else
    return nullptr;
}

// Returns true if the next token has type t.
inline bool
next_token_is(const Parser& p, Token_type t) {
  return peek(p)->type == t; 
}

// Returns true if the nth token has type t.
inline bool
nth_token_is(const Parser& p, std::size_t n, Token_type t) { 
  if (const Token* tok = lookahead(p, n))
    return tok->type == t;
  else
    return false;
}

// Returns the current location in the program source.
const Location&
location(const Parser& p) { return peek(p)->loc; }

// Return a diagnostic stream indicating an error at the current 
// position in thje program source.
inline std::ostream&
error(const Parser& p) { return error(location(p)); }

// Returns the current token, and advances the parser.
inline const Token*
consume(Parser& p) {
  const Token* tok = &*p.current;
  ++p.current;
  return tok;
}

// If the current token is of type T, advance to the next token
// and return true. Otherwise, return false.
const Token*
accept(Parser& p, Token_type t) {
  if (peek(p)->type == t)
    return consume(p);
  else
    return nullptr;
}

// Require the current token to match t, consuming it. Generate a
// diagnostic if the current token does not match.
const Token*
expect(Parser& p, Token_type t) {
  if (const Token* tok = accept(p, t))
    return tok;
  error(p) << "expected '" << spelling(t) << "'\n";
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Parser combinators and facilities

using Symbol = const Token* (*)(Parser&);
using Production = const Tree* (*)(Parser&);

// This template binds the production functions to the spelling
// of that production. It is used for diagnosing parse errors.
template<Production P>
  struct production;


// Either parse the given production or emit an error diagnosting the
// expression that is expected.
template<Production Prod>
  const Tree*
  parse_expected(Parser& p)
  {
    if (const Tree* n = Prod(p))
      return n;
    else 
      return nullptr;
  }

/// Parse a left-associative binary expression whose infix operator is
/// specified by parser Op, and whose left and right operand are parsed
/// by the productions Left and Right, respectively.
///
///    binary-left(Symbol Op, Production Sub) ::=
///      Sub [Op Sub]*
template<Symbol Op, Production Sub>
  const Tree*
  parse_binary_left(Parser& p) {
    if (const Tree* l = Sub(p)) {
      while (const Token* t = Op(p)) {
        if (const Tree* r = parse_expected<Sub>(p))
          l = p.make_binary(t, l, r);
        else
          return nullptr;
      }
      return l;
    }
    return nullptr;
  }

/// Parse a unary expression whose prefix operator specified by the
/// following production:
///
///   unary(Symbol Op, Production Self, Production Sub) ::=
///     Sub | Op Self
template<Symbol Op, Production Self, Production Sub>
  const Tree*
  parse_unary(Parser& p) {
    if (const Token* t = Op(p)) {
      if (const Tree* n = parse_expected<Self>(p))
        return p.make_unary(t, n);
      return nullptr;
    }
    return Sub(p);
  }

/// The kind of enclosing.
enum Enclosing {
  Paren_enc,
  Brace_enc,
  Bracket_enc,
  Angle_enc
};

static Token_type enclosing_tokens[][2] {
  {Left_paren_tok, Right_paren_tok},
  {Left_brace_tok, Right_brace_tok},
  {Left_bracket_tok, Right_bracket_tok},
  {Less_tok, Greater_tok}
};

inline Token_type
open_token(Enclosing e) { return enclosing_tokens[int(e)][0]; }

inline Token_type
close_token(Enclosing e) { return enclosing_tokens[int(e)][1]; }

template<Enclosing Enc, Production Prod>
  const Tree*
  parse_enclosed(Parser& p) {
    if (const Token* l = accept(p, open_token(Enc)))
      if (const Tree* n = parse_expected<Prod>(p))
        if (const Token* r = expect(p, close_token(Enc)))
          return p.make_enclosed(l, r, n);
    return nullptr;
  }

/// Parse a paren-enclosed production.
template<Production Prod>
  inline const Tree*
  parse_paren_enclosed(Parser& p) {
    return parse_enclosed<Paren_enc, Prod>(p);
  }

/// Parse a brace-enclosed production
template<Production Prod>
  inline const Tree*
  parse_brace_enclosed(Parser& p) {
    return parse_enclosed<Brace_enc, Prod>(p);
  }

/// Parse a bracket-enclosed production.
template<Production Prod>
  inline const Tree*
  parse_bracket_enclosed(Parser& p) {
    return parse_enclosed<Bracket_enc, Prod>(p);
  }

/// Parse a sequence of productions.
template<Production Prod>
  const Tree*
  parse_sequence(Parser& p) {
    Tree_sequence<> ts;
    while (const Tree* t = Prod(p)) {
      ts.push_back(t);
    }
    return p.make_sequence(std::move(ts));
  }


/// Parse a list of productions separated by the given token. Returns
/// a sequence of parsed elements.
template<Token_type T, Production Prod>
  const Tree*
  parse_separated(Parser& p) {
    Tree_sequence<> ts;
    while (const Tree* t = Prod(p)) {
      ts.push_back(t);
      if (accept(p, T))
        continue;
      else
        break;
    }
    return p.make_sequence(std::move(ts));
  }

/// Parse a comma-separated list of productions.
template<Production Prod>
  inline const Tree* 
  parse_comma_separated(Parser& p) {
    return parse_separated<Comma_tok, Prod>(p);
  }

// -------------------------------------------------------------------------- //
// Parser

// Names
const Tree* parse_identifier(Parser&);
const Tree* parse_name(Parser&);

// Expressions
const Tree* parse_default(Parser&);
const Tree* parse_boolean_lit(Parser&);
const Tree* parse_integer_lit(Parser&);
const Tree* parse_primary_expr(Parser&);
const Tree* parse_access_expr(Parser&, const Tree*);
const Tree* parse_call_expr(Parser&, const Tree*);
const Tree* parse_index_expr(Parser&, const Tree*);
const Tree* parse_postfix_expr(Parser&);
const Tree* parse_sign_expr(Parser&);
const Tree* parse_multiplicative_expr(Parser&);
const Tree* parse_additive_expr(Parser&);
const Tree* parse_ordering_expr(Parser&);
const Tree* parse_equality_expr(Parser&);
const Tree* parse_logical_not_expr(Parser&);
const Tree* parse_logical_and_expr(Parser&);
const Tree* parse_logical_or_expr(Parser&);
const Tree* parse_expr(Parser&);
const Tree* parse_type_expr(Parser&);
const Tree* parse_integral_expr(Parser&);

// Types
const Tree* parse_type(Parser&);
const Tree* parse_simple_type(Parser&);
const Tree* parse_class_type(Parser&);
const Tree* parse_variant_type(Parser&);
const Tree* parse_enum_type(Parser&);

// Classes
const Tree* parse_class_member(Parser&);
const Tree* parse_class_members(Parser&);
const Tree* parse_class_body(Parser&);

// Variants

// Enums

// Declarators
const Tree* parse_declarator(Parser&);
const Tree* parse_function_parameters(Parser&);
const Tree* parse_function_declarator(Parser&, const Tree*);
const Tree* parse_template_parameters(Parser&);
const Tree* parse_template_declarator(Parser&, const Tree*);

// Parameter and arguments
const Tree* parse_parameter_name(Parser&);
const Tree* parse_parameter(Parser&);
const Tree* parse_parameter_list(Parser&);
const Tree* parse_argument(Parser&);
const Tree* parse_argument_list(Parser&);

// Statements
const Tree* parse_def_stmt(Parser&);
const Tree* parse_print_stmt(Parser&);
const Tree* parse_scan_stmt(Parser&);
const Tree* parse_statement(Parser&);

const Tree* parse_program(Parser&);

/// identifier ::= [a-zA-Z_][a-zA-Z0-9_]*
const Tree*
parse_identifier(Parser& p) {
  if (const Token* t = accept(p, Identifier_tok))
    return p.make_identifier(t);
  return nullptr;
}

/// Parsing rules for names. When used in an exression a name refers to
/// a declaration. When used in a declarator, a name is introduced into
/// an enclosing scope.
///
///     name ::= identifier
///
/// \todo Undoubtedly, we will gain some special kinds of names. Those
/// should go here. Operator names will undoubtedly appear here.
const Tree*
parse_name(Parser& p) {
  return parse_identifier(p);
}

/// boolean-lit ::= 'true' | 'false'
const Tree*
parse_boolean_lit(Parser& p) {
  if (const Token* t = accept(p, True_tok))
    return p.make_literal(t);
  if (const Token* t = accept(p, False_tok))
    return p.make_literal(t);
  return nullptr;
}

/// integer-lit ::= [0-9]*
const Tree*
parse_integer_lit(Parser& p) {
  if (const Token* t = accept(p, Int_literal_tok))
    return p.make_literal(t);
  return nullptr;
}

/// Parse a default initializer.
///
///   default-init ::= 'default'
///
/// Like the name implies, the deault initializer can only appear
/// in an initialization context.
///
/// \todo: I think we may have an initializer production that is separate
/// from expression. Maybe it's possible to use default as a placeholder
/// for deduction in various contexts, or maybe we want real placeholders
/// for that purpose.
const Tree*
parse_default(Parser& p) {
  if (const Token* t = accept(p, Default_tok))
    return p.make_literal(t);
  return nullptr;
}

/// primary-expr ::= default
///                | boolean-lit 
///                | integer-lit
///                | type
///                | name
///                | '(' expression ')'
///
/// \todo This isn't quite right. I don't think we can parse dependent
/// names as a primary expression. That means that n(x : int) would be
/// a valid expression, and I don't believe that it is. Note that x(3)
/// is a postfix-expression, not a name.
const Tree*
parse_primary_expr(Parser& p) {
  if (const Tree* t = parse_default (p))
    return t;
  if (const Tree* t = parse_boolean_lit(p))
    return t;
  if (const Tree* t = parse_integer_lit(p))
    return t;
  if (const Tree* t = parse_type(p))
    return t;
  if (const Tree* t = parse_name(p))
    return t;
  if (const Tree* n = parse_enclosed<Paren_enc, parse_expr>(p))
    return n;
  return nullptr;
}

/// Parse an access expression.
///
///     access-expr ::= primary-expr | access-expr '.' name
///
/// \todo Can parse anything more complicated than a name following a dot?
/// This is to say, will the language support expressions that compute
/// the name or offset of a member? For example, maybe "s.0" returns the
/// first field of a product type? 
const Tree*
parse_access_expr(Parser& p, const Tree* t) {
  if (accept(p, Dot_tok))
    if (const Tree* mem = parse_name(p))
      return p.make_access(t, mem);
  return nullptr;
}

/// Parse a call expression.
///
///     call-expr ::= primary-expr | call-expr '(' [argument-list] ')'
const Tree*
parse_call_expr(Parser& p, const Tree* t) {
  if (const Tree* args = parse_paren_enclosed<parse_argument_list>(p))
    return p.make_call(t, args);
  return nullptr;
}

/// Parse an index expression.
///
///     index-expr ::= primary-expr | index-expr '[' expression ']'
const Tree*
parse_index_expr(Parser& p, const Tree* t) {
  if (const Tree* idx = parse_bracket_enclosed<parse_expr>(p))
    return p.make_index(t, idx);
  return nullptr;
}

/// Parse a postfix expression.
///
///    postfix-expr ::= access-expr | call-expr | subscript-expr
///
/// All postfix expressions have a primary expression as their first term.
const Tree*
parse_postfix_expr(Parser& p) {
  const Tree* t = parse_primary_expr(p);
  while (t) {
    if (const Tree* c = parse_call_expr(p, t))
      t = c;
    else if (const Tree* a = parse_access_expr(p, t))
      t = a;
    else if (const Tree* i = parse_index_expr(p, t))
      t = i;
    else
      break;
  }
  return t;
}

/// Parse an arithmetic sign operator.
///
///     sign-op ::= '+' | '-'
inline const Token*
parse_sign_op(Parser& p) {
  switch(peek(p)->type) {
  case Plus_tok:
  case Minus_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

/// Parse an arithmetic sign expression.
///
///     sign-expr ::= unary(sign-expr, postfix-expr, sign-op)
const Tree*
parse_sign_expr(Parser& p) {
  return parse_unary<parse_sign_op, parse_sign_expr, parse_postfix_expr>(p);
}

/// Parse a multiplicative operator.
///
///     multiplicative-op ::= '*' | '/' | '%'
inline const Token*
parse_multiplicative_op(Parser& p) {
  switch(peek(p)->type) {
  case Star_tok:
  case Slash_tok:
  case Percent_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

/// Parse a multiplicative expression.
///
///     multiplicative-expr ::= binary-left(sign-expr, multplicative-op)
const Tree*
parse_multiplicative_expr(Parser& p) {
  return parse_binary_left<parse_multiplicative_op, parse_sign_expr>(p);
}

/// Parse and additive operator.
///
///     additive-op ::= '+' | '-'
inline const Token*
parse_additive_op(Parser& p) {
  switch(peek(p)->type) {
  case Plus_tok:
  case Minus_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

/// Parse an additive expression.
///
///     additive-expr ::= binary-left(multiplicative-expr, additive-op)
const Tree*
parse_additive_expr(Parser& p) {
  return parse_binary_left<parse_additive_op, parse_multiplicative_expr>(p);
}

/// Parsse an relational ordering operator.
///
///     ordering-op ::= '<' | '>' | '<=' | '>='
inline const Token*
parse_ordering_op(Parser& p) {
  switch(peek(p)->type) {
  case Less_tok:
  case Greater_tok:
  case Less_equal_tok:
  case Greater_equal_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

/// Parse a relational ordering expression.
///
///     ordering-expr ::= binary-left(ordering-op, additive-expr)
const Tree*
parse_ordering_expr(Parser& p) {
  return parse_binary_left<parse_ordering_op, parse_additive_expr>(p);
}

/// Parse a relational equality operator.
///
///     equality-op ::= '==' | '!='
const Token*
parse_equality_op(Parser& p) {
  switch(peek(p)->type) {
  case Equal_equal_tok:
  case Bang_equal_tok:
    return consume(p);
  default:
    return nullptr;
  }
}

/// Parse a relational equality expression.
///     equality-expr ::= binary-left(equality-op, ordering-expr)
const Tree*
parse_equality_expr(Parser& p) {
  return parse_binary_left<parse_equality_op, parse_ordering_expr>(p);
}

/// Parse a logical not operator.
///
///     logical-not-op ::= 'not'
inline const Token*
parse_logical_not_op(Parser& p) { return accept(p, Not_tok); }

/// Parse a logical not expression.
///
///     logical-not-expr ::= unary(logical-not-op, equality-expr)
const Tree*
parse_logical_not_expr(Parser& p) {
  return parse_unary<parse_logical_not_op, 
                     parse_logical_not_expr, 
                     parse_equality_expr>(p);
}

/// Parse a logical and operator.
///
///     logical-and-operator ::= 'and'
inline const Token*
parse_logical_and_op(Parser& p) { return accept(p, And_tok); }

/// Parse a logical and expression.
///
///     logical-and-expression ::= binary-left(logical-and-op, logical-not-expr)
const Tree*
parse_logical_and_expr(Parser& p) {
  return parse_binary_left<parse_logical_and_op, parse_logical_not_expr>(p);
}

/// Parse a logical or operator
///
///     logical-or-operator ::= 'or'
inline const Token*
parse_logical_or_op(Parser& p) { return accept(p, Or_tok); }

/// Parse a logical or expression.
///
///     logical-or-expression ::= binary-left(logical-or-op, logical-and-expr)
const Tree*
parse_logical_or_expr(Parser& p) {
  return parse_binary_left<parse_logical_or_op, parse_logical_and_expr>(p);
}

/// Parse an expr.
///
///     expr ::= logical-or-expression
const Tree*
parse_expr(Parser& p) { 
  return parse_logical_or_expr(p); 
}

/// Parse a type expression. A type expressions is an expression that
/// denotes a valid type name.
///
///   type-expr ::= expr
const Tree*
parse_type_expr(Parser& p) { return parse_expr(p); }

/// Parse an integer width. This is an expresson that must reduce
/// to an integer value.
///
///     integral-expr ::= expr
const Tree*
parse_integral_expr(Parser& p) {
  return parse_expr(p);
}

/// Parse a simple type expression. A simple type expression denotes one of
/// the fundamental type abstractions in the Steve language.
///
///     simple-type ::= 'type' | 'bool' | 'int' | 'namespace'
///
/// Namespace is a type specifier. Like `type` it specifies a class of
/// expresssions denoted by a block of declarations.
const Tree*
parse_simple_type(Parser& p) {
  if (const Token* t = accept(p, Type_tok))
    return p.make_identifier(t);
  if (const Token* t = accept(p, Bool_tok))
    return p.make_identifier(t);
  if (const Token* t = accept(p, Int_tok))
    return p.make_identifier(t);
  if (const Token* t = accept(p, Namespace_tok))
    return p.make_identifier(t);
  return nullptr;
}

/// Parse a class member declaration.
///
///     class-member ::= declarator ':' type ';'
///
/// \todo Allow for non-static member initializers.
const Tree*
parse_class_member(Parser& p) {
  // FIXME: Return a member decl node.
  if (const Tree* name = parse_declarator(p))
    if (expect(p, Colon_tok))
      if (const Tree* type = parse_type(p))
        if (expect(p, Semicolon_tok))
          return type ? name : type;
  return nullptr;
}

/// Parse a sequence of class members.
///
///     class-membes ::= sequence(class-member)
const Tree*
parse_class_members(Parser& p) { 
  return parse_sequence<parse_class_member>(p);
}

/// Parse a class body.
///
///     class-body ::= '{' class-members '}'
const Tree*
parse_class_body(Parser& p) {
  return parse_brace_enclosed<parse_class_members>(p);
}

/// Parse a class type.
///
///   class-type ::= 'class' class-body
const Tree*
parse_class_type(Parser& p) {
  // FIXME: Return a class node
  if (accept(p, Class_tok))
    return parse_expected<parse_class_body>(p);
  return nullptr;
}

/// Parse a variation
///
///     basic-variation ::= type | name
///
/// \todo: This production is wrong. First, it should be clear that we 
/// want the name to refer to a type. Second, it should allow scoped 
/// references. Third, do we really want to allow types to be defined 
/// within the variant? 
const Tree* 
parse_variation(Parser& p) {
  if (const Tree* t = parse_type(p))
    return t;
  return parse_name(p);
}

/// Parse a basic variation
///
///     basic-variation ::= variation | 'default'
///
/// \todo: This production is wrong. First, it should be clear that we 
/// want the name to refer to a type. Second, it should allow scoped 
/// references. Third, do we really want to allow types to be defined 
/// within the variant? 
const Tree*
parse_basic_variation(Parser& p) {
  if (const Tree* t = parse_default(p))
    return t;
  else
    return parse_variation(p);
}

/// Parse a variation case in a dependent variation.
///
///     variation-case ::= postfix-expr '=>' variation
const Tree*
parse_variation_case(Parser& p) {
  /// FIXME: Return a match node.
  if (const Tree* expr = parse_postfix_expr(p))
    if (expect(p, Equal_greater_tok))
      if (const Tree* var = parse_variation(p))
        return var ? var : expr; // FIXME: WRONG
  return nullptr;
}

/// Parse a dependent variation.
///
///     dependent-variation ::= variant-case | 'default'
const Tree*
parse_dependent_variation(Parser& p) {
  if (const Tree* t = parse_default(p))
    return t;
  return parse_variation_case(p);
}

/// Parse a list of basic variations.
///
///     basic-variations ::= comma-separated(basic-variation)
const Tree*
parse_basic_variations(Parser& p) {
  return parse_comma_separated<parse_basic_variation>(p);
}

/// Parse a list of dependent variations.
///
///     dependent-variations ::= comma-separated(dependent-variation)
const Tree*
parse_dependent_variations(Parser& p) {
  return parse_comma_separated<parse_dependent_variation>(p);
}

/// Parse a basic variant body.
///
///     basic-variant-body ::= '{' basic-variations '}'
const Tree*
parse_basic_variant_body(Parser& p) {
  return parse_brace_enclosed<parse_basic_variations>(p);
}

/// Parse a dependent variant body.
///
///     dependent-variant-body ::= '{' basic-variations '}'
const Tree*
parse_dependent_variant_body(Parser& p) {
  return parse_brace_enclosed<parse_dependent_variations>(p);
}

/// Parse a basic variant type.
///
///     basic-variant-type ::= 'variant' basic-variant-body
const Tree*
parse_basic_variant_type(Parser& p) {
  // FIXME: Return a variant node.
  if (const Tree* body = parse_expected<parse_basic_variant_body>(p))
    return body; // WRONG
  return nullptr;
}

/// Parse a variant argument.
///
///     variant-argument ::= '(' identifier ')'
///
/// The identifier must refer to a parameter.
const Tree*
parse_variant_argument(Parser& p) {
  return parse_paren_enclosed<parse_identifier>(p);
}

/// Parse a dependent variant type.
///
///     dependent-variant-type :: 'variant' variant-argument variant-body
const Tree* 
parse_dependent_variant_type(Parser& p) {
  // FIXME: Return a variant node.
  if (const Tree* arg = parse_expected<parse_variant_argument>(p))
    if (const Tree* body = parse_expected<parse_dependent_variant_body>(p))
      return body ? body : arg; // WRONG
  return nullptr;
}

/// Parse a variant type.
///
///   variant-type ::= basic-variant-type | dependent-variant-type
///
/// \todo "Dependent" is not really the right name here. It's really a
/// remotely descriminated type. Maybe the name "matched" would be
/// appropriate.
const Tree*
parse_variant_type(Parser& p) {
  if (accept(p, Variant_tok)) {
    if (next_token_is(p, Left_paren_tok))
      return parse_dependent_variant_type(p);
    else
      return parse_basic_variant_type(p);
  }
  return nullptr;
}

///     enumerator ::= identifier ['=' expression]
const Tree*
parse_enumerator(Parser& p) {
  // FIXME: Implement me.
  return parse_identifier(p);
}

///     enumerators ::= comma-separated(enumerator)
const Tree*
parse_enumerators(Parser& p) {
  return parse_comma_separated<parse_enumerator>(p);
}

///     enum-body ::= '{' enumerators '}'
const Tree*
parse_enum_body(Parser& p) {
  return parse_brace_enclosed<parse_enumerators>(p);
}

///     basic-enum-type ::= 'enum' enum-body
const Tree*
parse_basic_enum_type(Parser& p) {
  // FIXME: Return an enum node
  return parse_expected<parse_enum_body>(p);
}

///     enum-base ::= '(' name ')'
///
/// \todo The `name` should be a type-type.
const Tree*
parse_enum_base(Parser& p) {
  return parse_paren_enclosed<parse_name>(p);
}

///     general-enum-type ::= 'enum' enum-base enum-body
const Tree*
parse_general_enum_type(Parser& p) {
  // FIXME: return an enum node.
  if (const Tree* base = parse_expected<parse_enum_base>(p))
    if (const Tree* body = parse_expected<parse_enum_body>(p))
      return body ? body : base;
  return nullptr;
}

/// Parse an enumerated type.
///
///     enum-type ::= basic-enum-type | general-enum-type
const Tree*
parse_enum_type(Parser& p) {
  if (accept(p, Enum_tok)) {
    if (next_token_is(p, Left_paren_tok))
      return parse_general_enum_type(p);
    else
      return parse_basic_enum_type(p);
  }
  return nullptr;
}

/// Parse a type expression.
///
///     type ::= simple-type 
///            | integral-type 
///            | class-type 
///            | variant-type 
///            | enum-type
///
/// A type expression is a primary expression, which means, from the 
/// grammatical perspective, it would be ok to write things like this:
///
///     def v : vector(record { x : int; y : int});
///
/// This instantiates the vector with an anonymous type. Whether or
/// not this is semantically meaningful is another question and probably
/// depends on whether or not v has external linkage.
const Tree*
parse_type(Parser& p) {
  if (const Tree* t = parse_simple_type(p))
    return t;
  if (const Tree* t = parse_class_type(p))
    return t;
  if (const Tree* t = parse_enum_type(p))
    return t;
  if (const Tree* t = parse_variant_type(p))
    return t;
  return nullptr;
}


/// Parse a function parameter list.
///
///     function-parameters ::= '(' parameter-list ')'
const Tree*
parse_function_parameters(Parser& p) {
  return parse_enclosed<Paren_enc, parse_parameter_list>(p);
}

/// Parse a function declarator.
///
///     function-declarator ::= name explicit-parameters
///
/// A function-declarator introduces a name whose definition is 
/// parameterized over a sequence of arguments whose types are determined
/// by function parameters.
const Tree*
parse_function_declarator(Parser& p, const Tree* n) {
  if (const Tree* f = parse_function_parameters(p))
    return p.make_function(n, f);
  return nullptr;
}

/// Parse a template parameter list.
///
///     template-parameters ::= '<' parameter-list '>'
const Tree*
parse_template_parameters(Parser& p) {
  return parse_enclosed<Angle_enc, parse_parameter_list>(p);
}

/// Parse a template declarator.
///
///     template-declarator ::= name template-parameters function-parameters
///
/// A template-declarator introduces a name whose definition is 
/// paramneterized over a sequence of template arguments and a sequence 
/// of function arguments. The template arguments are always deduced 
/// from the function arguments and corresponding function parameters.
const Tree*
parse_template_declarator(Parser& p, const Tree* n) {
  if (const Tree* t = parse_template_parameters(p))
    if (const Tree* f = parse_expected<parse_function_parameters>(p))
      return p.make_template(n, t, f);
  return nullptr;
}

/// Parse a declarator. A declarator is the name of declared entity. It
/// can be a simple identifier or something more complex.
///
///     declarator ::= object-declarator 
///                  | function-declarator 
///                  | template-declarator
///
///     object-declarator ::= name
///
///     function-declarator ::= name function-parameters
///
///     template-declarator ::= name template-parameters function-parameters
///
const Tree*
parse_declarator(Parser& p) {
  if (const Tree* n = parse_name(p)) {
    if (const Tree* t = parse_template_declarator(p, n))
      return t;
    if (const Tree* t = parse_function_declarator(p, n))
      return t;
    return p.make_value(n);
  }
  return nullptr;
}

/// Parse a parameter name.
///
///     parameter-name ::= name
///
/// \todo allow parameter names to be dependent. That is the mechanism needed
/// to support high order programming.
const Tree*
parse_parameter_name(Parser& p) {
  return parse_name(p);
}

/// Parse a parameter declaration.
///
///     parameter ::= parameter-name ':' type-expr ['=' expr]
///
/// \todo Support default arguments.
const Tree*
parse_parameter(Parser& p) {
  if (const Tree* name = parse_parameter_name(p))
    if (expect(p, Colon_tok))
      if (const Tree* type = parse_type_expr(p))
        return p.make_parameter(name, type);
  return nullptr;
}

/// Parse a parameter list.
///
///     parameter-list ::= comma-separated(p)
const Tree*
parse_parameter_list(Parser& p) {
  return parse_comma_separated<parse_parameter>(p);
}

/// Parse an argument to a call expression.
///
///     argument ::= expression
inline const Tree*
parse_argument(Parser& p) { return parse_expr(p); }

/// Parse an argument list.
///
///     argument-list ::= comma-separated(argument)
///
/// \todo: Refactor token-separated parsing into a combinator.
inline const Tree*
parse_argument_list(Parser& p) { 
  return parse_comma_separated<parse_argument>(p);
}

/// Parse a definition statement.
///
///     def-stmt ::= 'def' name ':' type-expr '=' expression ';'
const Tree*
parse_def_stmt(Parser& p) {
  if (accept(p, Def_tok))
    if (const Tree* decl = parse_expected<parse_declarator>(p))
      if (expect(p, Colon_tok))
        if (const Tree* ty = parse_expected<parse_type_expr>(p))
          if (expect(p, Equal_tok))
            if (const Tree* init = parse_expected<parse_expr>(p))
              if (expect(p, Semicolon_tok))
                return p.make_def(decl, ty, init);
  return nullptr;
}

/// Parse a scan statement.
///
///     scan-stmt ::= 'scan' name ':' type ';'
const Tree*
parse_scan_stmt(Parser& p) {
  if (accept(p, Scan_tok))
    if (const Tree* id = parse_expected<parse_name>(p))
      if (expect(p, Colon_tok))
        if (const Tree* ty = parse_expected<parse_type>(p))
          if (expect(p, Semicolon_tok))
            return p.make_scan(id, ty);
  return nullptr;
}

/// Parse a print statement.
///
///     print-stmt ::= 'print' expression ';'
const Tree*
parse_print_stmt(Parser& p) {
  if (accept(p, Print_tok))
    if (const Tree* e = parse_expected<parse_expr>(p))
      if (expect(p, Semicolon_tok))
        return p.make_print(e);
  return nullptr;
}


/// Parse a statement.
///
///     statement ::= def-stmt | print-stmt | scan-stmt
const Tree*
parse_statement(Parser& p) {
  if (const Tree* s = parse_def_stmt(p))
    return s;
  if (const Tree* s = parse_scan_stmt(p))
    return s;
  if (const Tree* s = parse_print_stmt(p))
    return s;
  return nullptr;
}

/// Parse a program.
///
///     program ::= statement*
///
/// \todo I'm not sure the name of the production is correct. It's not a
/// complete program, but it would be a complete module.
const Tree*
parse_program(Parser& p) {
  Tree_sequence<> stmts;
  while (const Tree* t = parse_statement(p))
    stmts.push_back(t);
  return p.make_program(std::move(stmts));
}

// -------------------------------------------------------------------------- //
// Parser

const Tree*
Parser::operator()() {
  if (current != tokens.end())
    return parse_expected<parse_program>(*this);
  else
    return nullptr;
}

} // namespace steve
