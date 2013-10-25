

set(src Utility.cpp Memory.cpp String.cpp Ios.cpp Integer.cpp)
set(hdr Utility.hpp Memory.hpp String.hpp Ios.hpp Integer.hpp)

# Cofigure binaries.
add_library(sarah_utility STATIC ${src})
