#ifndef STEVE_PARSER_HPP
#define STEVE_PARSER_HPP

#include <iostream>

#include "Ast.hpp"

namespace steve {

// -------------------------------------------------------------------------- //
// Factory implementation

const Enclosed_tree* 
Tree_factory::make_enclosed(const Token* l, const Token* r, const Tree* n) {
  return encs.make(l, r, n);
}

const Sequence_tree*
Tree_factory::make_sequence(Tree_sequence<>&& s) {
  return seqs.make(std::move(s));
}

const Literal_tree* 
Tree_factory::make_literal(const Token* t) { 
  return lits.make(t); 
}

const Identifier_tree*
Tree_factory::make_identifier(const Token* t) {
  return ids.make(t);
}

const Unary_tree* 
Tree_factory::make_unary(const Token* o, const Tree* e) {
  return uns.make(o, e);
}

const Binary_tree* 
Tree_factory::make_binary(const Token* o, const Tree* l, const Tree* r) {
  return bins.make(o, l, r);
}

const Call_tree*
Tree_factory::make_call(const Tree* f, const Tree* a) {
  return calls.make(f, a);
}

const Access_tree*
Tree_factory::make_access(const Tree* s, const Tree* m) {
  return accs.make(s, m);
}

const Index_tree*
Tree_factory::make_index(const Tree* s, const Tree* i) {
  return idxs.make(s, i);
}

const Type_tree*
Tree_factory::make_type(const Token* t) {
  return types.make(t);
}

const Value_tree*
Tree_factory::make_value(const Tree* t) {
  return vals.make(t);
}

const Function_tree*
Tree_factory::make_function(const Tree* n, const Tree* p) {
  return fns.make(n, p);
}

const Template_tree*
Tree_factory::make_template(const Tree* n, const Tree* i, const Tree* e) {
  return tmpls.make(n, i, e);
}

const Parameter_tree*
Tree_factory::make_parameter(const Tree* n, const Tree* t) {
  return params.make(n, t);
}

const Def_tree*
Tree_factory::make_def(const Tree* n, const Tree* t, const Tree* i) {
  return defs.make(n, t, i);
}

const Scan_tree*
Tree_factory::make_scan(const Tree* n, const Tree* t) {
  return scans.make(n, t);
}

const Print_tree*
Tree_factory::make_print(const Tree* e) {
  return prints.make(e);
}

const Program_tree*
Tree_factory::make_program(Tree_sequence<>&& s) {
  return progs.make(std::move(s));
}

} // namespace steve


#endif
