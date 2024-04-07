cmake_minimum_required(VERSION 2.8)

set(name pps_test)

project(name)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_QNX_SOURCE")

include_directories(${PROJECT_SOURCE_DIR}/include/)

add_executable(pub pub.cpp)
target_link_libraries(pub -pthread pps)

add_executable(sub sub.cpp)
target_link_libraries(sub -pthread pps)

get_target_property(DEPENDED_LIBS_DIRS ${name} LINK_DIRECTORIES)
message(DEPENDED_LIBS_DIRS= ${DEPENDED_LIBS_DIRS})
