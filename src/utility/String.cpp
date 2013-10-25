
#include <unordered_set>

#include "String.hpp"

// The string table.
using String_table = std::unordered_set<std::string>;
static String_table strings;

namespace sarah {

// Returns a pointer to a unique string with the same spelling as str.
const std::string* 
String::intern(const std::string& str) {
  return &*strings.insert(str).first;
}

} // namespace sarah
