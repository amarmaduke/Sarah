
#include <iostream>

#include <semantics/Language.hpp>
#include <semantics/Debug.hpp>

using namespace std;
using namespace sarah;

int main() {
  Context cxt;

  Id& x = cxt.make_id("x");
  Bind& b = cxt.make_bind(x, cxt.int_type);
  Expr& e = cxt.make_neg(x); // -x
  Expr& f = cxt.make_forall(b, e);
  cout << f << '\n';

  /*
  const Var& a = cxt.make_var("a");
  const Var& b = cxt.make_var("b");
  const Expr& e = cxt.make_exists(a, b);
  */
}
