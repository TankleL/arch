cmake_minimum_required (VERSION 3.0)

FILE(GLOB_RECURSE TINYXML2_INC_FILES "inc/*.h")
aux_source_directory (src TINYXML2_SRC_FILES)

include_directories(inc)

add_library(tinyxml2_a STATIC ${TINYXML2_INC_FILES} ${TINYXML2_SRC_FILES})

