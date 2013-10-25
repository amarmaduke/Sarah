

set(src Language.cpp Debug.cpp)
set(hdr Language.hpp Debug.hpp)

# Configure binaries.
add_library(sarah_language STATIC ${src})

