add_library(unicode
    neo/unicode.hpp
    neo/unicode/code_unit_buffer.hpp
    neo/unicode/unicode.hpp
    neo/unicode/encodings/all.hpp
    neo/unicode/encodings/encodings.hpp
    neo/unicode/encodings/utf8.hpp
    neo/unicode/encodings/utf16.hpp
    neo/unicode/encodings/utf16.cpp
    neo/unicode/encodings/utf32.hpp
    neo/unicode/encodings/wide.hpp
    neo/unicode/encodings/wide.cpp
    )

set_target_properties(unicode PROPERTIES
    OUTPUT_NAME neo-unincode
    EXPORT_NAME neo::unicode
    )
target_include_directories(unicode PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(unicode PRIVATE utf8rewind)
target_compile_features(unicode PUBLIC
    cxx_alias_templates
    cxx_auto_type
    cxx_attributes
    cxx_constexpr
    cxx_decltype
    cxx_deleted_functions
    cxx_return_type_deduction
    )
add_library(neo::unicode ALIAS unicode)

set(NEO_UNICODE_LIBRARY_NAME unicode)
set(NEO_UNICODE_LIBRARY_INTERFACE PUBLIC)
include(ExtraConfig.cmake)

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