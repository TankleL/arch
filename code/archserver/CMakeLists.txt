#archserver level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " archserver)

include_directories(inc)
file(GLOB_RECURSE ARCHSVR_INC_FILES "inc/*.hpp")
aux_source_directory (src ARCHSVR_SRC_FILES)

include_directories(inc/core)
file(GLOB_RECURSE ARCHSVR_CORE_INC_FILES "inc/core/*.hpp")
aux_source_directory(src/core ARCHSVR_CORE_SRC_FILES)

include_directories(inc/config "inc/config/*.hpp")
file(GLOB_RECURSE ARCHSVR_CONFIG_INC_FILES)
aux_source_directory(src/config ARCHSVR_CONFIG_SRC_FILES)

include_directories(inc/ipro "inc/ipro/*.hpp")
file(GLOB_RECURSE ARCHSVR_IPRO_INC_FILES)
aux_source_directory(src/ipro ARCHSVR_IPRO_SRC_FILES)

include_directories(inc/svc "inc/svc/*.hpp")
file(GLOB_RECURSE ARCHSVR_SVC_INC_FILES)
aux_source_directory(src/svc ARCHSVR_SVC_SRC_FILES)

include_directories(inc/exceptions "inc/exceptions/*.hpp")
file(GLOB_RECURSE ARCHSVR_EXCEPTIONS_INC_FILES)
aux_source_directory(src/svc ARCHSVR_EXCEPTIONS_SRC_FILES)

include_directories(deps/libuv/include)
include_directories(deps/tinyxml2/inc)

add_subdirectory(deps/libuv)
add_subdirectory(deps/tinyxml2)

add_compile_definitions(TEST_PATH_CONFIG_FILE="${TEST_ENV_ROOT_PATH}/archserver/svr_config.xml")
add_compile_definitions(TEST_PATH_RES_MODULES_ROOT="${TEST_ENV_ROOT_PATH}/modules")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(	archsvr
#------ header files
	${ARCHSVR_INC_FILES}
	${ARCHSVR_CORE_INC_FILES}
	${ARCHSVR_CONFIG_INC_FILES}
	${ARCHSVR_IPRO_INC_FILES}
	${ARCHSVR_SVC_INC_FILES}
	${ARCHSVR_EXCEPTIONS_INC_FILES}
#------ source files
	${ARCHSVR_SRC_FILES}
	${ARCHSVR_CORE_SRC_FILES}
	${ARCHSVR_CONFIG_SRC_FILES}
	${ARCHSVR_IPRO_SRC_FILES}
	${ARCHSVR_SVC_SRC_FILES}
	${ARCHSVR_EXCEPTIONS_SRC_FILES})

target_link_libraries( archsvr uv_a )
target_link_libraries( archsvr tinyxml2_a )

#if(WIN32)
	#set(ARCHSVR_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-win32.bat ${ARCHSVR_INC} ${LIBAM_INC})
#else()
	#set(ARCHSVR_POST_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-linux.sh)
#endif()

#message("post build commands: " ${ARCHSVR_POST_BUILD_COMMAND})
#add_custom_command(TARGET archsvr POST_BUILD
	#COMMAND ${ARCHSVR_POST_BUILD_COMMAND})


message("leave-level < " archserver)
