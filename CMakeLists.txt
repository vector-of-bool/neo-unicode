cmake_minimum_required(VERSION 3.5.0)
project(neo-unicode VERSION 0.0.0)

include(CTest)
enable_testing()

include(utf8rewind.cmake)

add_subdirectory(source)

if(BUILD_TESTING)
    get_filename_component(EXTERN_DIR extern ABSOLUTE)
    get_filename_component(catch_hpp extern/catch/catch.hpp ABSOLUTE)
    add_subdirectory(tests)
endif()

add_executable(unicode-example example.cpp)
target_link_libraries(unicode-example PRIVATE neo::unicode)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
