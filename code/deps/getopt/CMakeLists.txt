
cmake_minimum_required (VERSION 3.0)

FILE(GLOB_RECURSE GETOPT_INC_FILES "inc/*.h")
aux_source_directory (src GETOPT_SRC_FILES)

include_directories(inc)

add_library(getopt_a STATIC
    ${GETOPT_INC_FILES}
    ${GETOPT_SRC_FILES}
)

