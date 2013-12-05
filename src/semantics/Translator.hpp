
#ifndef SARAH_TRANSLATOR_HPP
#define SARAH_TRANSLATOR_HPP

#include "Language.hpp"
#include "Elaborator.hpp"

namespace sarah {

struct Elaboration;

struct Translator
{
  // Context to use
  Context& context;

  Translator(Context& con)
    : context(con)
  { }

  Elaboration operator()(const Expr&);
  Elaboration translate(const Expr&, bool);

};

} // namespace sarah

#endif
