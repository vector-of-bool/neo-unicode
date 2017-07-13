get_filename_component(ur_dir extern/utf8rewind ABSOLUTE)

file(GLOB ur_sources
    ${ur_dir}/source/*.c
    ${ur_dir}/source/internal/*.c
    )

add_library(utf8rewind INTERFACE)

target_include_directories(utf8rewind INTERFACE ${ur_dir}/include/utf8rewind ${ur_dir}/source)
target_sources(utf8rewind INTERFACE ${ur_sources})