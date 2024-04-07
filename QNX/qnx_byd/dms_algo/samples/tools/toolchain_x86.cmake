#---------------------------------------------------------------
# CPPBUILD ToolChain related Settings.
# Note: include this fine before 'project(...)' command in samples
#---------------------------------------------------------------
#set toolchains.
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER "/usr/bin/cc")
SET(CMAKE_CXX_COMPILER "/usr/bin/c++")
SET(CMAKE_CXX_COMPILER_ID GNU)
SET(CMAKE_CXX_COMPILER_VERSION 4.9.4)
SET(CMAKE_CUDA_HOST_COMPILER )

#currently we only support samples on linux, windows, android, and macos
#MSVC will be set automatically on windows
SET(UNIX 1)
SET(APPLE )
SET(IOS )
SET(ANDROID )
SET(ANDROID_ARM64 )

SET(CPPBUILD_CXX_DIALECT c++11)
SET(CPPBUILD_TARGET_OS LINUX)
SET(CPPBUILD_TARGET_ARCH X86_64)
SET(CPPBUILD_TARGET_CPU_TYPE )
SET(CPPBUILD_TARGET_OS_VERSION )
SET(CPPBUILD_TARGET_CUDA_VERSION 8.0)
SET(CPPBUILD_TARGET_CUDNN_VERSION 5.0)
SET(CPPBUILD_TARGET_OCL_VERSION )
SET(CPPBUILD_TARGET_SNPE_VERSION )

