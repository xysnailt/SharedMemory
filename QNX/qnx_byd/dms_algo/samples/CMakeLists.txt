#=========================================================================================
#See http://confluence.bj.sensetime.com/display/INFRAS/SDK+v6.0
#for generic help about how to run sdk
#======================================================================================
cmake_minimum_required(VERSION 3.12)

#============ User Defined Options ================

#Change to ´Debug´ to compile debug version of binary.
#Note: This does not work on windows. see README.md
set(CMAKE_BUILD_TYPE Release)

#Set the lib path of the internal sdk you want to use.
#set(LIB_FOLDER_NAME lib)		#use dynamic version internal sdk(dev)
set(LIB_FOLDER_NAME lib_static)	#use static version internal sdk(dev)
#set(LIB_FOLDER_NAME libs/linux-x86_64)		#use dynamic version internal sdk, in a FORMALLY RELEASED package
#set(LIB_FOLDER_NAME libs_static/linux-x86_64)  #use static version internal sdk, in a FORMALLY RELEASED package

#Register 'ppl_cpu' by enable following, BEWARE that only do this when you are using static
#libs sdk and The SDK you applied support this engine(Compiled),
#but not registered(CONFIG_SDK_MODEL_ENABLE_PPL is false)
#SET(USE_SDK_MODEL_PPL TRUE)
#ADD_DEFINITIONS(-DUSE_SDK_MODEL_PPL)

#Register 'ppl_cuda' by enable following, BEWARE that only do this when you are using static
#libs sdk and The SDK you applied support this engine(Compiled),
#but not registered(CONFIG_SDK_MODEL_ENABLE_CUDA is false)
#SET(USE_SDK_MODEL_CUDA TRUE)
#ADD_DEFINITIONS(-DUSE_SDK_MODEL_CUDA)

#Register 'ppl_ocl' by enable following, BEWARE that only do this when you are using static
#libs sdk and The SDK you applied support this engine(Compiled),
#but not registered(CONFIG_SDK_MODEL_ENABLE_OCL is false)
#SET(USE_SDK_MODEL_OCL TRUE)
#ADD_DEFINITIONS(-DUSE_SDK_MODEL_OCL)

#Register 'ppl_hardcode' by enable following, BEWARE that only do this when you are using static
#libs sdk and The SDK you applied support this engine(Compiled),
#but not registered(CONFIG_SDK_MODEL_ENABLE_HARDCODE is false)
#SET(USE_SDK_MODEL_HARDCODE TRUE)
#ADD_DEFINITIONS(-DUSE_SDK_MODEL_HARDCODE)

#Register 'ppl_dsp' by enable following, BEWARE that only do this when you are using static
#libs sdk and The SDK you applied support this engine(Compiled),
#but not registered(CONFIG_SDK_MODEL_ENABLE_DSP is false)
#SET(USE_SDK_MODEL_DSP TRUE)
#ADD_DEFINITIONS(-DUSE_SDK_MODEL_DSP)

#Register 'snpe' by enable following, BEWARE that only do this when you are using static
#libs sdk and The SDK you applied support this engine(Compiled),
#but not registered(CONFIG_SDK_MODEL_ENABLE_SNPE is false)
#SET(USE_SDK_MODEL_SNPE TRUE)
#ADD_DEFINITIONS(-DUSE_SDK_MODEL_SNPE)
#=================== END ====================

set(CMAKE_BUILD_TYPE Release)

macro(CPPBUILD_APPEND_CFLAGS extraflags)
	#SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${extraflags} -fsanitize=address -fno-omit-frame-pointer -D_GLIBCXX_SANITIZE_VECTOR")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${extraflags} -fno-omit-frame-pointer -D_GLIBCXX_SANITIZE_VECTOR")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${extraflags}")
	#SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${extraflags} -fsanitize=address -fno-omit-frame-pointer")
	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${extraflags} -fno-omit-frame-pointer")
	SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${extraflags}")
endmacro()

set(CPPBUILD_IMPORT_TOOLCHAIN_CONFIG TRUE)
SET(CMAKE_EXE_LINKER_FLAGS -pie)

if(LIB_FOLDER_NAME MATCHES "_static")
	SET(depend_target_name sensedriver)
	MESSAGE("Build against static lib")
else()
	IF(CPPBUILD_TARGET_OS STREQUAL "WINDOWS")
		SET(depend_target_name sensedriver_dyn_shared)
	ELSE()
		SET(depend_target_name sensedriver_shared)
	ENDIF()
	ADD_DEFINITIONS(-DUSE_SHARED_INTERNAL_SDK)
	MESSAGE("Build against shared lib")
endif()

#Must include toolchain.cmake before PROJECT command
include(${CMAKE_CURRENT_SOURCE_DIR}/tools/toolchain.cmake)
PROJECT(sensedriver_samples)
if(NOT MSVC)
	MESSAGE("Build configuration \"${CMAKE_BUILD_TYPE}\"")
endif()

#include(${CMAKE_CURRENT_SOURCE_DIR}/../../${LIB_FOLDER_NAME}/sensedriver_export.cmake)

SET(CMAKE_INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/opencv")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
FILE(GLOB samples ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB json11 ${PROJECT_SOURCE_DIR}/../samples/tools/json11/json11.cpp)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/tools/opencv/include)
SET(DEMO_SHARED_LIBS "")
IF(CPPBUILD_TARGET_OS STREQUAL "WINDOWS")
	FILE(GLOB OPENCV_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/tools/opencv/lib/*.lib)
ELSE()
	FILE(GLOB OPENCV_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/tools/opencv/lib/*.so)
ENDIF()

LIST(APPEND DEMO_SHARED_LIBS ${OPENCV_LIBS})
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../lib)

## fix android-aarch64 link error for ocl and snpe
## the libopencl.so and libsnpe.so checked in git depends on more third party libs, and might not be the
## correct version, add this flag so when we build test, it won't fail because of undefined symbol.
## Especially, the libopencl.so shall NEVER be copied to target machine to run, because it is a wrong version.

IF(NOT CPPBUILD_TARGET_OS STREQUAL "WINDOWS" )
	CPPBUILD_APPEND_CFLAGS("-Wl,--allow-shlib-undefined -O3 -DNDEBUG -fvisibility=hidden -fPIC -Wno-unused-local-typedefs -std=gnu++11")
    #SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -fvisibility=hidden -fPIC -Wno-unused-local-typedefs -std=gnu++11")
ELSE()
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /utf-8")
	#include(${CMAKE_CURRENT_SOURCE_DIR}/tools/m_closewarning.cmake)
ENDIF()


FOREACH (sample ${samples})
	STRING(REGEX MATCH "[^/]+$" sample_file ${sample})
	STRING(REPLACE ".cpp" "" sample_basename ${sample_file})
	ADD_EXECUTABLE(test_${sample_basename} ${sample} ${json11})

	TARGET_LINK_LIBRARIES(test_${sample_basename} ${depend_target_name} ${DEMO_SHARED_LIBS})
    IF(CPPBUILD_TARGET_OS STREQUAL "LINUX")
        IF(NOT CPPBUILD_TARGET_CPU_TYPE STREQUAL "qnx")
            TARGET_LINK_LIBRARIES(test_${sample_basename} pthread)
        ENDIF()
    ENDIF()

	INSTALL(TARGETS test_${sample_basename} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
ENDFOREACH()
#INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/opencv/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/opencv)
