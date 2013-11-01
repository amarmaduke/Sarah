
#include <iostream>

#include "Diagnostics.hpp"
#include "Location.hpp"

namespace steve {

inline std::ostream&
diag(const char* kind, const Location& loc) 
{ 
  return std::cerr << loc << ": " << kind << ": ";
}


std::ostream&
error(const Location& loc) { return diag("error", loc); }

std::ostream&
warning(const Location& loc) { return diag("warning", loc); }

std::ostream&
note(const Location& loc) { return diag("note", loc); }

} // namespace steve
