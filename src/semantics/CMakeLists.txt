

set(src Language.cpp Elaborator.cpp Debug.cpp)
set(hdr Language.hpp Elaborator.hpp Debug.hpp)

add_library(sarah_language STATIC ${src})

