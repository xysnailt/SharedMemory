cmake_minimum_required(VERSION 2.8)

set(name senseauto_dms_service)
project(name)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--disable-new-dtags -D_QNX_SOURCE")

# add_definitions("-Wall -g")
# SET(CMAKE_BUILD_TYPE "Debug")

link_directories(${PROJECT_SOURCE_DIR}/deps/ais_client/lib/)
link_directories(${PROJECT_SOURCE_DIR}/deps/pm_lib/lib/)
link_directories(${PROJECT_SOURCE_DIR}/dms_algo/lib/)

include_directories(${PROJECT_SOURCE_DIR}/deps/ais_client/inc/)
include_directories(${PROJECT_SOURCE_DIR}/deps/pm_lib/include/)
include_directories(${PROJECT_SOURCE_DIR}/dms_algo/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/common)

FILE(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${name} ${SRC})

target_link_libraries(${name} -pthread ais_client libstd mmap_peer pps sv_oem_pm_client_lib
OSAbstraction pmem_client pmemext slog2 socket sensedriver)

get_target_property(DEPENDED_LIBS_DIRS ${name} LINK_DIRECTORIES)
message(DEPENDED_LIBS_DIRS= ${DEPENDED_LIBS_DIRS})

#create compile info text
execute_process(
COMMAND git rev-parse --short=8 HEAD
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
OUTPUT_VARIABLE _GIT_COMMIT
ERROR_QUIET
OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
COMMAND date +%y%m%d.%H%M
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
OUTPUT_VARIABLE _COMPLITE_TIME
ERROR_QUIET
OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(COMPLITE_INFO "dms_service_${_GIT_COMMIT}_${_COMPLITE_TIME}")
add_definitions(-DCOMPLITE_INFO=\"${COMPLITE_INFO}\")
execute_process(
COMMAND touch BYDD9/OUTPUT/${COMPLITE_INFO}.txt
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
ERROR_QUIET
OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS " COMPLITE_INFO: ${COMPLITE_INFO}")