add_library(nonius::nonius INTERFACE IMPORTED)
set_target_properties(nonius::nonius PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/extern/nonius"
    INTERFACE_COMPILE_FEATURES cxx_std_14
    )
