#Top level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > top level")

project( arch )

SET(ARCHSVR_SHARED_INC ${CMAKE_CURRENT_SOURCE_DIR}/archserver/shared_inc)

add_subdirectory( archserver )
add_subdirectory( module-demo )

message("leave-level < top level")