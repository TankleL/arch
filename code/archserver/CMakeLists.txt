#archserver level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " archserver)

FILE(GLOB_RECURSE ARCHSVR_INC_FILES "inc/*.hpp")

aux_source_directory (src ARCHSVR_SRC_FILES)

include_directories(inc)
include_directories(deps/libuv/include)
include_directories(deps/tinyxml2/inc)

add_subdirectory(deps/libuv)
add_subdirectory(deps/tinyxml2)

add_compile_definitions(TEST_PATH_CONFIG_FILE="${TEST_ENV_ROOT_PATH}/archserver/svr_config.xml")
add_compile_definitions(TEST_PATH_RES_MODULES_ROOT="${TEST_ENV_ROOT_PATH}/modules")

add_executable( archsvr ${ARCHSVR_INC_FILES} ${ARCHSVR_SINC_FILES} ${ARCHSVR_SRC_FILES} )
target_link_libraries( archsvr uv_a )
target_link_libraries( archsvr tinyxml2_a )

if(WIN32)
	set(ARCHSVR_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-win32.bat ${ARCHSVR_INC} ${LIBAM_INC})
else()
	set(ARCHSVR_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-linux.sh)
endif()

message("post build commands: " ${ARCHSVR_POST_BUILD_COMMAND})
add_custom_command(TARGET archsvr POST_BUILD
	COMMAND ${ARCHSVR_POST_BUILD_COMMAND})


message("leave-level < " archserver)
