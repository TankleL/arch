#archservice level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > antennamsg")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(inc)
file(GLOB_RECURSE ANTENNAMSG_INC_FILES "inc/*.hpp")
aux_source_directory (src ANTENNAMSG_SRC_FILES)

add_library(atmsg_a STATIC
#------ header files
    ${ANTENNAMSG_INC_FILES}
#------ source files
    ${ANTENNAMSG_SRC_FILES}
)

message("leave-level < antennamsg")
