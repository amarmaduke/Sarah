
#include <cassert>

#include "syntax/Tree.hpp"
#include "Elaborator.hpp"
#include "Language.hpp"


namespace sarah {

namespace {

Elaboration
elab_enclosed(Elaborator& elab, const Enclosed_tree& tree) {
  return elab(tree.arg());
}

// FIXME: This is wrong! We need to find the binding of the identifier
// in order to get the right type.
Elaboration
elab_id(Elaborator& elab, const Token& tok) {
  return {elab.make_id(tok.spell), elab.int_type};
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

Elaboration
elab_terminal(Elaborator& elab, const Terminal_tree& tree) {
  switch (tree.token().type) {
  case Identifier_tok: return elab_id(elab, tree.token());
  case True_tok: return elab_bool(elab, true);
  case False_tok: return elab_bool(elab, false);
  case Int_tok: return elab_int(elab, tree.token());
  default:
    assert(false); // Unreachable
  }
  return Elaboration();
}

Elaboration
elab_unary(Elaborator& elab, const Unary_tree& tree) {
  switch (tree.op().type) {
  case Minus_tok:
  case Plus_tok:
  case Not_tok:

  default:
    assert(false); // Unreachable
  }
  return Elaboration();
}

Elaboration
elab_bind(Elaborator& elab, const Binary_tree& tree) {
  // The name must be transformed into an Id.
  // const Tree& name = tree.left();

  // We could just get the spelling and match type type names
  // specifically. A more general approach would be to elaborate
  // the type id and see what we get.
  // const Tree& type = tree.right();

  // Register the declaration.
  // elab.declare(n, t);

  // Return a new binding.
  // return {elab.make_bind(n, t), t};
  return Elaboration();
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
