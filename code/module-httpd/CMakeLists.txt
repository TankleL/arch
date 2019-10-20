#module-httpd level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " module-httpd)


FILE(GLOB_RECURSE MODULE_HTTPD_INC_FILES "inc/*.hpp")
aux_source_directory (src MODULE_HTTPD_SRC_FILES)

include_directories(inc)
include_directories(${LIBAM_INC})

add_library(mdlhttpd SHARED ${MODULE_HTTPD_INC_FILES} ${MODULE_HTTPD_SRC_FILES})
target_link_libraries(mdlhttpd libam_a)

if(WIN32)
	set(MODULE_HTTPD_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-win32.bat   ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${TEST_ENV_ROOT_PATH})
else()
	set(MODULE_HTTPD_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-linux.sh)
endif()

message("post build commands: " ${MODULE_HTTPD_POST_BUILD_COMMAND})
add_custom_command(TARGET mdlhttpd POST_BUILD
	COMMAND ${MODULE_HTTPD_POST_BUILD_COMMAND})

message("leave-level < " module-httpd)
