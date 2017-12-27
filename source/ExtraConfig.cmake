include(CMakePushCheckState)
include(CheckCXXSourceCompiles)

cmake_push_check_state(RESET)

set(concept_src [[
template <typename> concept bool foo() { return true; }
int main() {}
]])

check_cxx_source_compiles("${concept_src}" NEO_HAVE_CXX_CONCEPTS)

set(CMAKE_REQUIRED_FLAGS -fconcepts)
check_cxx_source_compiles("${concept_src}" NEO_HAVE_CXX_CONCEPTS_FCONCEPTS)

cmake_pop_check_state()

if(NEO_HAVE_CXX_CONCEPTS OR NEO_HAVE_CXX_CONCEPTS_FCONCEPTS)
    option(NEO_UNICODE_ENABLE_CONCEPTS "Enable C++ concepts" ${NEO_UNICODE_IS_ROOT})
else()
    option(NEO_UNICODE_ENABLE_CONCEPTS "Enable C++ concepts [unsupported with this compiler]" FALSE)
endif()

target_compile_definitions(
    ${NEO_UNICODE_LIBRARY_NAME} ${NEO_UNICODE_LIBRARY_INTERFACE}
    $<BUILD_INTERFACE:NEO_UNICODE_ENABLE_CONCEPTS=$<BOOL:${NEO_UNICODE_ENABLE_CONCEPTS}>>
    )

if(NEO_UNICODE_ENABLE_CONCEPTS)
    if(NEO_HAVE_CXX_CONCEPTS)
        # Nothing to do!
    elseif(NEO_HAVE_CXX_CONCEPTS_FCONCEPTS)
        target_compile_options(${NEO_UNICODE_LIBRARY_NAME} ${NEO_UNICODE_LIBRARY_INTERFACE} -fconcepts)
    else()
        message(SEND_ERROR "Requested concepts support from neo::unicode, but concepts are not supported by the compiler (${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})")
    endif()
endif()
