#archserver level cmakelists
cmake_minimum_required (VERSION 3.0)

FILE(GLOB_RECURSE ARCHSVR_INC_FILES "inc/*.hpp")
aux_source_directory (src ARCHSVR_SRC_FILES)

include_directories(inc)
include_directories(deps/libuv/include)
include_directories(deps/tinyxml2/inc)

add_subdirectory(deps/libuv)
add_subdirectory(deps/tinyxml2)

add_executable( archsvr ${ARCHSVR_INC_FILES} ${ARCHSVR_SRC_FILES} )
target_link_libraries( archsvr uv_a )
target_link_libraries( archsvr tinyxml2_a )
