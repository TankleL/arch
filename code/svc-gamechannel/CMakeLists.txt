
#svc-gamechannel level cmakelists
cmake_minimum_required (VERSION 3.0)
message("enter-level > svc-gamechannel")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${LIBUV_INC})
include_directories(${ARCHSVC_INC})

include_directories(inc)
file(GLOB_RECURSE SVC_GCHANNEL_INC_FILES "inc/*.hpp")
aux_source_directory (src SVC_GCHANNEL_SRC_FILES)

add_executable(svc_gamechannel
#------ header files
    ${SVC_GCHANNEL_INC_FILES}
#------ source files
    ${SVC_GCHANNEL_SRC_FILES}
)

target_link_libraries(svc_gamechannel
    uv_a
    archsvc_a
)

message("leave-level < svc-gamechannel")
