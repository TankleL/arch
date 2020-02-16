#Top level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > top level")

project( arch )

set(ARCHSVR_INC			${CMAKE_CURRENT_SOURCE_DIR}/archserver/inc)
set(TEST_ENV_ROOT_PATH	${CMAKE_CURRENT_SOURCE_DIR}/../test_env)
set(LIBUV_INC           ${CMAKE_CURRENT_SOURCE_DIR}/deps/libuv/include)
set(TINYXML2_INC        ${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2/inc)
set(ARCHSVC_INC         ${CMAKE_CURRENT_SOURCE_DIR}/archservice/inc)

add_subdirectory( deps/libuv )
add_subdirectory( deps/tinyxml2 )
add_subdirectory( archserver )
add_subdirectory( archservice )
add_subdirectory( svc-gamechannel )

message("leave-level < top level")