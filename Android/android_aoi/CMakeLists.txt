cmake_minimum_required(VERSION 2.8)

set(name dmsServer)

project(name)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--disable-new-dtags -D_QNX_SOURCE")

link_directories(${PROJECT_SOURCE_DIR}/ais_client/lib/)
link_directories(${PROJECT_SOURCE_DIR}/dms_algo/lib/)

include_directories(${PROJECT_SOURCE_DIR}/ais_client/inc/)
include_directories(${PROJECT_SOURCE_DIR}/dms_algo/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/)

FILE(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${name} ${SRC})

target_link_libraries(${name} -pthread ais_client libstd mmap_peer pps
OSAbstraction pmem_client pmemext slog2 socket sensedriver)

get_target_property(DEPENDED_LIBS_DIRS ${name} LINK_DIRECTORIES)
message(DEPENDED_LIBS_DIRS= ${DEPENDED_LIBS_DIRS})
