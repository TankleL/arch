#module-demo level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " module-demo)

FILE(GLOB_RECURSE MODULE_DEMO_INC_FILES "inc/*.hpp")
aux_source_directory (src MODULE_DEMO_SRC_FILES)

message("path of archsvr-shared-inc: " ${ARCHSVR_SHARED_INC})

include_directories(inc)
include_directories(${ARCHSVR_SHARED_INC})

add_library(mdldemo SHARED ${MODULE_DEMO_INC_FILES} ${MODULE_DEMO_SRC_FILES})

if(WIN32)
	set(MODULE_DEMO_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-win32.bat   ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${TEST_ENV_ROOT_PATH})
else()
	set(MODULE_DEMO_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-linux.sh)
endif()

message("post build commands: " ${MODULE_DEMO_POST_BUILD_COMMAND})
add_custom_command(TARGET mdldemo POST_BUILD
	COMMAND ${MODULE_DEMO_POST_BUILD_COMMAND})

message("leave-level < " module-demo)
