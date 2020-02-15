#libam level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > " libam)

FILE(GLOB_RECURSE LIBAM_INC_FILES "inc/*.hpp")
aux_source_directory (src LIBAM_SRC_FILES)

include_directories(inc)

add_library(libam_a STATIC ${LIBAM_SRC_FILES} ${LIBAM_INC_FILES})

message("leave-level < libam")
