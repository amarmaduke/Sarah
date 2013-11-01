

set(src Token.cpp Lexer.cpp)
set(hdr Token.hpp Lexer.hpp)

add_library(sarah_syntax STATIC ${src})

