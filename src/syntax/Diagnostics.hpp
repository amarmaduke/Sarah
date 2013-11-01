#ifndef SETEVE_DIAGNOSTICS_HPP
#define SETEVE_DIAGNOSTICS_HPP

#include <iosfwd>

namespace steve {

struct Location;

std::ostream& error(const Location&);
std::ostream& warning(const Location&);
std::ostream& note(const Location&);

} // namespace steve

#endif
