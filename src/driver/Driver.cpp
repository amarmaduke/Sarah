
#include <iostream>

#include <semantics/Language.hpp>
#include <semantics/Debug.hpp>

using namespace std;
using namespace sarah;




int main() {
  Context cxt;

  auto& x = cxt.make_neg(cxt.make_id("x"));
  auto& e = cxt.make_add(cxt.make_int(3), x);
  cout << e << '\n';  

  /*
  const Var& a = cxt.make_var("a");
  const Var& b = cxt.make_var("b");
  const Expr& e = cxt.make_exists(a, b);
  */
}
