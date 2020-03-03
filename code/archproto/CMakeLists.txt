#archservice level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > archproto")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${VARDATA_INC})

include_directories(inc)
file(GLOB_RECURSE ARCHPROTO_INC_FILES "inc/*.hpp")
aux_source_directory (src ARCHPROTO_SRC_FILES)

add_library(archproto_a STATIC
#------ header files
    ${ARCHPROTO_INC_FILES}
#------ source files
    ${ARCHPROTO_SRC_FILES}
)

target_link_libraries(archproto_a
        vardata_a)

message("leave-level < archproto")
