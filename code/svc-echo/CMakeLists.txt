
#svc-gamechannel level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > svc-echo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LIBUV_INC})
include_directories(${ARCHSVC_INC})
include_directories(${GETOPT_INC})
include_directories(${VARDATA_INC})
include_directories(${ARCHPROTO_INC})

include_directories(inc)
file(GLOB_RECURSE SVC_ECHO_INC_FILES "inc/*.hpp")
aux_source_directory (src SVC_ECHO_SRC_FILES)

add_executable(svc_echo
#------ header files
    ${SVC_ECHO_INC_FILES}
#------ source files
    ${SVC_ECHO_SRC_FILES}
)

target_link_libraries(svc_echo
    uv_a
    archsvc_a
    getopt_a
)

#post build
if(WIN32)
	set(SVC_ECHO_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-win32.bat ${PROJECT_BINARY_DIR} ${GSVR_ENV_ROOT})
else()
	set(SVC_ECHO_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-linux.sh)
endif()

message("post build commands: " ${SVC_ECHO_BUILD_COMMAND})
add_custom_command(TARGET svc_echo POST_BUILD
	COMMAND ${SVC_ECHO_BUILD_COMMAND})

message("leave-level < svc-echo")
