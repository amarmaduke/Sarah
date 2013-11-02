
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

// NOTE: An identifier found in any context where an identifier is
// not explicitly introduced is always elaborated as a var.
Elaboration
elab_terminal(Elaborator& elab, const Terminal_tree& tree) {
  const Token& tok = tree.token();
  switch (tok.type) {
  case Identifier_tok: return elab_var(elab, tok);
  case True_tok: return elab_bool(elab, true);
  case False_tok: return elab_bool(elab, false);
  case Int_literal_tok: return elab_int(elab, tok);
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

  // Elaborate the type. 
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

Elaboration
elab_binary(Elaborator& elab, const Binary_tree& tree) {
  switch (tree.op().type) {
  case Colon_tok:
    return elab_bind(elab, tree);

  // Arithmetic operators
  case Plus_tok:
  case Minus_tok:
  case Star_tok:

  // Relational operators
  case Equal_equal_tok:
  case Not_equal_tok:
  case Less_tok:
  case Greater_tok:
  case Less_equal_tok:
  case Greater_equal_tok:

  // Logical operators
  case And_tok:
  case Or_tok:
  case Imp_tok:
  case Iff_tok:

  // Quantiifers
  case Forall_tok:
  case Exists_tok:

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
