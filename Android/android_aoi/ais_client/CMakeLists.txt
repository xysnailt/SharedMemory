cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_COMPILER "/usr/local/toolchains/__android-ndk-r18b/clang-ollvm-str/android-21/android-aarch64/bin/clang")
SET(CMAKE_CXX_COMPILER "/usr/local/toolchains/__android-ndk-r18b/clang-ollvm-str/android-21/android-aarch64/bin/clang++")

set(name ais_test)

project(name)

message("test")



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

ADD_DEFINITIONS("")

link_directories(${PROJECT_SOURCE_DIR}/lib/)

include_directories(${PROJECT_SOURCE_DIR}/inc/)
# include_directories(${PROJECT_SOURCE_DIR}/inc/include)

SET(SOURCES ${PROJECT_SOURCE_DIR}/ais_test.cpp)

FILE(GLOB DEP_LIBS ${PROJECT_SOURCE_DIR}/lib/*.so)

add_executable(${name} ${SOURCES})

target_link_libraries(${name} ${DEP_LIBS})

# libstd mmap_peer
# OSAbstraction socket)

get_target_property(DEPENDED_LIBS_DIRS ${name} LINK_DIRECTORIES)
message(depended_libs_dirs= ${DEPENDED_LIBS_DIRS})
