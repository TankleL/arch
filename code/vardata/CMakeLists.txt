#archservice level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > vardata")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(inc)
file(GLOB_RECURSE VARDATA_INC_FILES "inc/*.hpp")
aux_source_directory (src VARDATA_SRC_FILES)

add_library(vardata_a STATIC
#------ header files
    ${VARDATA_INC_FILES}
#------ source files
    ${VARDATA_SRC_FILES}
)

message("leave-level < vardata")
