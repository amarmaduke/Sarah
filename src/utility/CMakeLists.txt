

set(src Utility.cpp 
        Memory.cpp 
        String.cpp 
        Ios.cpp 
        Integer.cpp 
        Location.cpp 
        File.cpp)

set(hdr Utility.hpp 
        Memory.hpp 
        String.hpp 
        Ios.hpp 
        Integer.hpp 
        Locatoin.hpp 
        File.hpp)

add_library(sarah_utility STATIC ${src})
