cmake_minimum_required(VERSION 2.8)

set(name ais_test)

project(name)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

ADD_DEFINITIONS("")

link_directories(${PROJECT_SOURCE_DIR}/lib/)

include_directories(${PROJECT_SOURCE_DIR}/inc/)

SET(SOURCES ${PROJECT_SOURCE_DIR}/ais_test/ais_test.cpp)

FILE(GLOB DEP_LIBS ${PROJECT_SOURCE_DIR}/lib/*.so)

add_executable(${name} ${SOURCES} ${DEP_LIBS})

target_link_libraries(${name} -pthread ais_client libstd mmap_peer
OSAbstraction pmem_client pmemext slog2 socket)

get_target_property(DEPENDED_LIBS_DIRS ${name} LINK_DIRECTORIES)
message(DEPENDED_LIBS_DIRS= ${DEPENDED_LIBS_DIRS})
