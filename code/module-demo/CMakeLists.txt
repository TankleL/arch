#module-demo level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " module-demo)

FILE(GLOB_RECURSE MODULE_DEMO_INC_FILES "inc/*.hpp")
aux_source_directory (src MODULE_DEMO_SRC_FILES)

message("path of archsvr-shared-inc: " ${ARCHSVR_SHARED_INC})

include_directories(inc)
include_directories(${ARCHSVR_SHARED_INC})

add_library(mdldemo SHARED ${MODULE_DEMO_INC_FILES} ${MODULE_DEMO_SRC_FILES})

message("leave-level < " module-demo)
