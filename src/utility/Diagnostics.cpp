
#include <iostream>

#include "Diagnostics.hpp"
#include "Location.hpp"

namespace sarah {

namespace {
inline std::ostream&
diag(const char* kind) {
  return std::cerr << kind << ": ";
}

inline std::ostream&
diag(const char* kind, const Location& loc) {
  return std::cerr << loc << ": " << kind << ": ";
}
}

std::ostream&
error() { return diag("error"); }

std::ostream&
error(const Location& loc) { return diag("error", loc); }

std::ostream&
warning() { return diag("warning"); }

std::ostream&
warning(const Location& loc) { return diag("warning", loc); }

std::ostream&
note() { return diag("note"); }

std::ostream&
note(const Location& loc) { return diag("note", loc); }

} // namespace sarah
