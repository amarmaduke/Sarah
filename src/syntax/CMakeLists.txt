

set(src Token.cpp Lexer.cpp Parser.cpp Tree.cpp Sexpr.cpp)
set(hdr Token.hpp Lexer.hpp Parser.hpp Tree.hpp Sexpr.hpp)

add_library(sarah_syntax STATIC ${src})

