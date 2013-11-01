
set(src Driver.cpp)
set(libs sarah_language sarah_syntax  sarah_utility ${GMP_LIBRARIES})

add_executable(sarah ${src})
target_link_libraries(sarah ${libs})

