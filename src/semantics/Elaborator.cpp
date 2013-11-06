
#include <cassert>
#include <iostream>

#include "utility/Diagnostics.hpp"
#include "syntax/Tree.hpp"
#include "syntax/Sexpr.hpp"

#include "Elaborator.hpp"
#include "Language.hpp"
#include "Debug.hpp"


namespace sarah {

namespace {

Elaboration
elab_enclosed(Elaborator& elab, const Enclosed_tree& tree) {
  return elab(tree.arg());
}

// Returns the declaration corresponding the given name by searching
// through the stack.
Elaboration
elab_var(Elaborator& elab, const Token& tok) {
  String str = tok.spell;
  if (const Decl* d = elab.lookup(str))
    return {elab.make_var(d->name, *d), d->type};
  error(tok.loc) << "no such declaration '" << str << "'\n";
  return {};
}

Elaboration
elab_bool(Elaborator& elab, bool b) {
  return {elab.make_bool(b), elab.bool_type};
}

Elaboration
elab_int(Elaborator& elab, const Token& tok) {
  Integer n = tok.spell;
  return {elab.make_int(n), elab.int_type};
}

// A type name designates a type definition, so return a variable that
// refers to that definition. This seems overly complicated, but if
// we had user-defined types, it would make a lot of sense.
Elaboration
elab_type(Elaborator& elab, const Def& d) {
  return {elab.make_var(d.name, d), d.type};
}

// NOTE: An identifier found in any context where an identifier is
// not explicitly introduced is always elaborated as a var.
Elaboration
elab_terminal(Elaborator& elab, const Terminal_tree& tree) {
  const Token& tok = tree.token();
  switch (tok.type) {

  // Identifiers and literals
  case Identifier_tok: return elab_var(elab, tok);
  case True_tok: return elab_bool(elab, true);
  case False_tok: return elab_bool(elab, false);
  case Int_literal_tok: return elab_int(elab, tok);

  // Types
  case Bool_tok: return elab_type(elab, *elab.bool_def);
  case Int_tok: return elab_type(elab, *elab.int_def);

  default:
    assert(false); // Unreachable
  }
  return {};
}

bool
check_type(Elaborator& elab, Elaboration e, const Def& def) {
  if(same(e.type(), def.init))
    return true;

  // FIXME: Improve this diagnostic.
  error() << "expression does not have type '" << def.name << "'\n";
  return false;
}

// Helper functions for creating elaborations.
Elaboration
make_neg(Elaborator& elab, Elaboration e) {
  return {elab.make_neg(e.expr()), elab.int_type};
}

Elaboration
make_pos(Elaborator& elab, Elaboration e) {
  return {elab.make_pos(e.expr()), elab.int_type};
}

Elaboration
make_not(Elaborator& elab, Elaboration e) {
  return {elab.make_not(e.expr()), elab.bool_type};
}

// A helper function for elaborating unary expressions.
template<Elaboration(*Make)(Elaborator&, Elaboration)>
  Elaboration
  elab_unary(Elaborator& elab, const Unary_tree& tree, const Def& type) {
  if (Elaboration e = elab(tree.arg())) {
    if (check_type(elab, e, type))
      return Make(elab, e);
  }
  return {};
}

Elaboration
elab_neg(Elaborator& elab, const Unary_tree& tree) {
  return elab_unary<make_neg>(elab, tree, *elab.int_def);
}

Elaboration
elab_pos(Elaborator& elab, const Unary_tree& tree) {
  return elab_unary<make_pos>(elab, tree, *elab.int_def);
}

Elaboration
elab_not(Elaborator& elab, const Unary_tree& tree) {
  return elab_unary<make_not>(elab, tree, *elab.bool_def);
}

Elaboration
elab_unary(Elaborator& elab, const Unary_tree& tree) {
  switch (tree.op().type) {
  case Minus_tok: return elab_neg(elab, tree);
  case Plus_tok: return elab_pos(elab, tree);
  case Not_tok: return elab_not(elab, tree);

  default:
    assert(false); // Unreachable
  }
  return Elaboration();
}

// Create an Id from the terminal node. The terminal node
// must represent an identifier.
const Id&
make_name(Elaborator& elab, const Terminal_tree& tree) {
  assert(tree.token().type == Identifier_tok);
  return elab.make_id(tree.token().spell);
}

// TODO: This is gross. Break it into smaller functions.
Elaboration
elab_bind(Elaborator& elab, const Binary_tree& tree) {
  // The name must be transformed into an Id. The syntax
  // guarantees that is an identifier.
  const Id& n = make_name(elab, as<Terminal_tree>(tree.left()));

  // Elaborate the type expression.
  if (Elaboration e = elab(tree.right())) {
    if (const Var* v = as<Var>(&e.expr())) {

      // Ensure that we have a definition
      if (const Def* d = as<Def>(&v->decl())) {

        // Check that the definition actually designates a type.
        if (const Type* t = as<Type>(&d->init))
          return {elab.make_bind(n, *t), *t};
        error() << "name '" << v->name() << "' does not designate a type\n";
        return {};
      }

      // If we didn't have a definition, then the type is incomplete.
      // This will never happen in this language.
      error() << "declaring '" << n << "' with incomplete type '"
              << v->decl().name << "'\n";
      return {};
    }

    // If we supported more complex types, we could bind a variable
    // to a more complex type expression.
    assert(false); // Unreachable
  }

  return {};
}

// Helper functions for creating elaborations
Elaboration
make_add(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_add(e1.expr(), e2.expr()), elab.int_type};
}

Elaboration
make_sub(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_add(e1.expr(), e2.expr()), elab.int_type};
}

Elaboration
make_mul(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_add(e1.expr(), e2.expr()), elab.int_type};
}

Elaboration
make_div(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_add(e1.expr(), e2.expr()), elab.int_type};
}

Elaboration
make_eq(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_eq(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_ne(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_ne(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_lt(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_lt(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_gt(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_gt(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_le(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_le(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_ge(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_ge(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_and(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_and(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_or(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_or(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_imp(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_imp(e1.expr(), e2.expr()), elab.bool_type};
}

Elaboration
make_iff(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_iff(e1.expr(), e2.expr()), elab.bool_type};
}

// A helper function for elaborating binary expressions. Here, both
// subexpressions must have type t.
template<Elaboration(*Make)(Elaborator&, Elaboration, Elaboration)>
  Elaboration
  elab_binary(Elaborator& elab, const Binary_tree& tree, const Def& t) {
    if (Elaboration e1 = elab(tree.left()))
      if (Elaboration e2 = elab(tree.right()))
        if (check_type(elab, e1, t) and check_type(elab, e2, t))
          return Make(elab, e1, e2);
    return {};
  }


Elaboration
elab_add(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_add>(elab, tree, *elab.int_def);
}

Elaboration
elab_sub(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_sub>(elab, tree, *elab.int_def);
}

// TODO: I'm not sure what to do with this. Maybe we should
// automatically expand it and not actually represent multiplication
// in the abstract language.
Elaboration
elab_mul(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_mul>(elab, tree, *elab.int_def);
//return {};
}

// Test implementation
Elaboration
elab_div(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_div>(elab, tree, *elab.int_def);
}

Elaboration
elab_eq(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_eq>(elab, tree, *elab.int_def);
}

Elaboration
elab_ne(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_ne>(elab, tree, *elab.int_def);
}

Elaboration
elab_lt(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_lt>(elab, tree, *elab.int_def);
}

Elaboration
elab_gt(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_gt>(elab, tree, *elab.int_def);
}

Elaboration
elab_le(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_le>(elab, tree, *elab.int_def);
}

Elaboration
elab_ge(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_ge>(elab, tree, *elab.int_def);
}

Elaboration
elab_and(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_and>(elab, tree, *elab.bool_def);
}

Elaboration
elab_or(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_or>(elab, tree, *elab.bool_def);
}

Elaboration
elab_imp(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_imp>(elab, tree, *elab.bool_def);
}

Elaboration
elab_iff(Elaborator& elab, const Binary_tree& tree) {
  return elab_binary<make_iff>(elab, tree, *elab.bool_def);
}

// A helper class for managing scopes during elaboration. This
// creates a new environment that is pushed onto the stack when
// constructed and popped when it's destroyed.
struct Quantifier_scope {
  Quantifier_scope(Elaborator& e)
    : elab(e), env() { elab.push(env); }

  ~Quantifier_scope() { elab.pop(); }

  Elaborator& elab;
  Environment env;
};

// Helper functions for creating elaborations
Elaboration
make_forall(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_forall(as<Bind>(e1.expr()), e2.expr()), elab.bool_type };
}

Elaboration
make_exists(Elaborator& elab, Elaboration e1, Elaboration e2) {
  return {elab.make_exists(as<Bind>(e1.expr()), e2.expr()), elab.bool_type };
}

template<Elaboration (*Make)(Elaborator&, Elaboration, Elaboration)>
  Elaboration
  elab_quantifier(Elaborator& elab, const Binary_tree& tree) {
    // Elaborate the binding. If it failed, bail out. Otherwise,
    // the syntax guarantees that e1 is a bind expression.
    Elaboration e1 = elab(tree.left());
    if (not e1)
      return e1;
    const Bind& b = as<Bind>(e1.expr());

    // Create a new environment, push it onto the stack
    // and register the binding.
    Quantifier_scope scope = elab;
    elab.declare(b.name(), b.type());

    // Now, elaborate the 2nd expression and ensure that it
    // is a boolean expression.
    if (Elaboration e2 = elab(tree.right()))
      if (check_type(elab, e2, *elab.bool_def))
        return Make(elab, e1, e2);
    return {};
  }

Elaboration
elab_forall(Elaborator& elab, const Binary_tree& tree) {
  return elab_quantifier<make_forall>(elab, tree);
}

Elaboration
elab_exists(Elaborator& elab, const Binary_tree& tree) {
  return elab_quantifier<make_exists>(elab, tree);
}

Elaboration
elab_binary(Elaborator& elab, const Binary_tree& tree) {
  switch (tree.op().type) {
  // Arithmetic operators
  case Plus_tok: return elab_add(elab, tree);
  case Minus_tok: return elab_sub(elab, tree);
  case Star_tok: return elab_mul(elab, tree);
  case Div_tok: return elab_div(elab, tree);

  // Relational operators
  case Equal_equal_tok: return elab_eq(elab, tree);
  case Not_equal_tok: return elab_ne(elab, tree);
  case Less_tok: return elab_lt(elab, tree);
  case Greater_tok: return elab_gt(elab, tree);
  case Less_equal_tok: return elab_le(elab, tree);
  case Greater_equal_tok: return elab_ge(elab, tree);

  // Logical operators
  case And_tok: return elab_and(elab, tree);
  case Or_tok: return elab_or(elab, tree);
  case Imp_tok: return elab_imp(elab, tree);
  case Iff_tok: return elab_iff(elab, tree);

  // Quantifiers
  case Colon_tok: return elab_bind(elab, tree);
  case Forall_tok: return elab_forall(elab, tree);
  case Exists_tok: return elab_exists(elab, tree);

  default:
    assert(false); // Unreachable
  }
  return Elaboration();
}

} // namespace

Elaboration
Elaborator::operator()(const Tree& tree) {
  struct V : Tree::Visitor {
    V(Elaborator& e)
      : elab(e) { }

    void visit(const Enclosed_tree& tree) {
      result = elab_enclosed(elab, tree);
    }

    void visit(const Terminal_tree& tree) {
      result = elab_terminal(elab, tree);
    }

    void visit(const Unary_tree& tree) {
      result = elab_unary(elab, tree);
    }

    void visit(const Binary_tree& tree) {
      result = elab_binary(elab, tree);
    }

    Elaborator& elab;
    Elaboration result;
  };

  V vis(*this);
  tree.accept(vis);
  return vis.result;
}


} // namespace sarah
