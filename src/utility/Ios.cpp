
#include <iostream>

#include "Ios.hpp"

namespace sarah {

int
stream_base(const std::ios_base& s) {
  std::ios_base::fmtflags f = s.flags() & std::ios_base::basefield;
  if (f == std::ios_base::hex)
    return 16;
  else if (f == std::ios_base::oct)
    return 8;
  else
    return 10;
}

} // namespace sarah
