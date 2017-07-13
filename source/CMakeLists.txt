add_library(unicode
    neo/unicode.hpp
    neo/unicode/bytes.hpp
    neo/unicode/encoders.hpp
    neo/unicode/unicode.hpp
    neo/unicode/unicode.cpp
    neo/unicode/views.hpp
    )

set_target_properties(unicode PROPERTIES
    OUTPUT_NAME neo-unincode
    EXPORT_NAME neo::unicode
)
target_include_directories(unicode PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(unicode PRIVATE utf8rewind)
add_library(neo::unicode ALIAS unicode)

if(WIN32)
    set(runtime_dir bin)
else()
    set(runtime_dir lib)
endif()

install(TARGETS unicode
    EXPORT neo-unicode
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION ${runtime_dir}
    INCLUDES DESTINATION include
    )