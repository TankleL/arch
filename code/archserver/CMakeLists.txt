#archserver level cmakelists

FILE(GLOB_RECURSE INC_FILES "inc/*.hpp")
aux_source_directory (src SRC_FILES)

include_directories(inc)
add_executable( archsvr ${INC_FILES} ${SRC_FILES} )
