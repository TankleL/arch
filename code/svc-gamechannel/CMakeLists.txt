
#svc-gamechannel level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > svc-gamechannel")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LIBUV_INC})
include_directories(${GETOPT_INC})
include_directories(${VARDATA_INC})
include_directories(${ARCHSVC_INC})
include_directories(${ARCHPROTO_INC})

include_directories(inc)
file(GLOB_RECURSE SVC_GCHANNEL_INC_FILES "inc/*.hpp")
aux_source_directory (src SVC_GCHANNEL_SRC_FILES)

include_directories(inc/config)
file(GLOB_RECURSE SVC_GCHANNEL_CONFIG_INC_FILES "inc/config/*.hpp")
aux_source_directory (src/config SVC_GCHANNEL_CONFIG_SRC_FILES)

include_directories(inc/messages)
file(GLOB_RECURSE SVC_GCHANNEL_MESSAGES_INC_FILES "inc/messages/*.hpp")
aux_source_directory (src/messages SVC_GCHANNEL_MESSAGES_SRC_FILES)

include_directories(inc/service)
file(GLOB_RECURSE SVC_GCHANNEL_SERVICE_INC_FILES "inc/service/*.hpp")
aux_source_directory (src/service SVC_GCHANNEL_SERVICE_SRC_FILES)

add_executable(svc_gamechannel
#------ header files
    ${SVC_GCHANNEL_INC_FILES}
    ${SVC_GCHANNEL_CONFIG_INC_FILES}
    ${SVC_GCHANNEL_MESSAGES_INC_FILES}
    ${SVC_GCHANNEL_SERVICE_INC_FILES}
#------ source files
    ${SVC_GCHANNEL_SRC_FILES}
    ${SVC_GCHANNEL_CONFIG_SRC_FILES}
    ${SVC_GCHANNEL_MESSAGES_SRC_FILES}
    ${SVC_GCHANNEL_SERVICE_SRC_FILES}
)

target_link_libraries(svc_gamechannel
    uv_a
    archsvc_a
    getopt_a
)

#post build
if(WIN32)
	set(SVC_GAMECHANNEL_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-win32.bat ${PROJECT_BINARY_DIR} ${GSVR_ENV_ROOT})
else()
	set(SVC_GAMECHANNEL_BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildtools/post-build-cmd-linux.sh)
endif()

message("post build commands: " ${SVC_GAMECHANNEL_BUILD_COMMAND})
add_custom_command(TARGET svc_gamechannel POST_BUILD
	COMMAND ${SVC_GAMECHANNEL_BUILD_COMMAND})

message("leave-level < svc-gamechannel")
