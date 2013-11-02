#ifndef SARHA_DIAGNOSTICS_HPP
#define SARHA_DIAGNOSTICS_HPP

#include <iosfwd>

namespace sarah {

struct Location;

std::ostream& error();
std::ostream& error(const Location&);

std::ostream& warning();
std::ostream& warning(const Location&);

std::ostream& note();
std::ostream& note(const Location&);

} // namespace sarah

#endif
