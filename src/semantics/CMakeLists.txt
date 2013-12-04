

set(src Language.cpp Elaborator.cpp Debug.cpp Translator.cpp)
set(hdr Language.hpp Elaborator.hpp Debug.hpp Translator.hpp)

add_library(sarah_language STATIC ${src})

