#archservice level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > archservice")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LIBUV_INC})
include_directories(${GETPOT_INC})

include_directories(inc)
file(GLOB_RECURSE ARCHSVC_INC_FILES "inc/*.hpp")
aux_source_directory (src ARCHSVC_SRC_FILES)

add_library(archsvc_a STATIC
#------ header files
    ${ARCHSVC_INC_FILES}
#------ source files
    ${ARCHSVC_SRC_FILES}
)

target_link_libraries(archsvc_a
    uv_a
    getopt_a
)


message("leave-level < archservice")
