
#include "Tree.hpp"

namespace sarah {

Enclosed_tree& 
Tree::Factory::make_enclosed(const Token& l, const Token& r, const Tree& n) {
  return encs.make(l, r, n);
}

Literal_tree& 
Tree::Factory::make_literal(const Token& t) { 
  return lits.make(t); 
}

Identifier_tree&
Tree::Factory::make_identifier(const Token& t) {
  return ids.make(t);
}

Unary_tree& 
Tree::Factory::make_unary(const Token& o, const Tree& e) {
  return uns.make(o, e);
}

Binary_tree& 
Tree::Factory::make_binary(const Token& o, const Tree& l, const Tree& r) {
  return bins.make(o, l, r);
}

} // namespace sarah

