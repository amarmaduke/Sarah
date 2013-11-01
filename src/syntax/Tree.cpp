
#include "Tree.hpp"

namespace sarah {

Enclosed_tree& 
Tree::Factory::make_enclosed(const Token& l, const Token& r, const Tree& n) {
  return encs.make(l, r, n);
}

Terminal_tree& 
Tree::Factory::make_terminal(const Token& t) { 
  return terms.make(t); 
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

