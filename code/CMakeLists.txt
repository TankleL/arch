#Top level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > top level")

project( arch )

set(LIBUV_INC           ${CMAKE_CURRENT_SOURCE_DIR}/deps/libuv/include)
set(TINYXML2_INC        ${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2/inc)
set(GETOPT_INC          ${CMAKE_CURRENT_SOURCE_DIR}/deps/getopt/inc)
set(ARCHSVC_INC         ${CMAKE_CURRENT_SOURCE_DIR}/archservice/inc)
set(GSVR_ENV_ROOT       ${CMAKE_CURRENT_BINARY_DIR}/../../gsvr)

add_subdirectory( deps/getopt )
add_subdirectory( deps/libuv )
add_subdirectory( deps/tinyxml2 )
add_subdirectory( archserver )
add_subdirectory( archservice )
add_subdirectory( svc-echo)
add_subdirectory( svc-gamechannel )

message("leave-level < top level")