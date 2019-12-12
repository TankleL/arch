#archserver level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " archserver)

FILE(GLOB_RECURSE ARCHSVR_INC_FILES "inc/*.hpp")
FILE(GLOB_RECURSE ARCHSVR_SINC_FILES "shared_inc/*.hpp")

aux_source_directory (src ARCHSVR_SRC_FILES)

include_directories(inc)
include_directories(shared_inc/)
include_directories(deps/libuv/include)
include_directories(deps/tinyxml2/inc)

add_subdirectory(deps/libuv)
add_subdirectory(deps/tinyxml2)

add_compile_definitions(TEST_PATH_CONFIG_FILE="${TEST_ENV_ROOT_PATH}/archserver/svr_config.xml")
add_compile_definitions(TEST_PATH_RES_MODULES_ROOT="${TEST_ENV_ROOT_PATH}/modules")

add_executable( archsvr ${ARCHSVR_INC_FILES} ${ARCHSVR_SINC_FILES} ${ARCHSVR_SRC_FILES} )
target_link_libraries( archsvr uv_a )
target_link_libraries( archsvr tinyxml2_a )



message("leave-level < " archserver)
