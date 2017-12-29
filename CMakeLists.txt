cmake_minimum_required(VERSION 3.5.0)
project(neo-unicode VERSION 0.0.0)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    set(NEO_UNICODE_IS_ROOT TRUE)
endif()

include(CTest)
enable_testing()

include(utf8rewind.cmake)
include(nonius.cmake)

add_subdirectory(source)
add_subdirectory(examples)

if(BUILD_TESTING)
    get_filename_component(EXTERN_DIR extern ABSOLUTE)
    get_filename_component(catch_hpp extern/catch/catch.hpp ABSOLUTE)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
