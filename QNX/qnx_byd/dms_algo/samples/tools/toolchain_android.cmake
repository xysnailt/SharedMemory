#---------------------------------------------------------------
# CPPBUILD ToolChain related Settings.
# Note: include this fine before 'project(...)' command in samples
#---------------------------------------------------------------
#set toolchains.
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER "/usr/local/toolchains/__android-ndk-r18b/clang-ollvm-str/android-21/android-aarch64/bin/clang")
SET(CMAKE_CXX_COMPILER "/usr/local/toolchains/__android-ndk-r18b/clang-ollvm-str/android-21/android-aarch64/bin/clang++")
SET(CMAKE_CXX_COMPILER_ID Clang)
SET(CMAKE_CXX_COMPILER_VERSION 7.0.1)
SET(CMAKE_CUDA_HOST_COMPILER )


#currently we only support samples on linux, windows, android, and macos
#MSVC will be set automatically on windows
SET(UNIX 1)
SET(APPLE )
SET(IOS )
SET(ANDROID )
SET(ANDROID_ARM64 )

SET(CPPBUILD_CXX_DIALECT c++11)
SET(CPPBUILD_USE_STD_CXX_LIB )
SET(CPPBUILD_TARGET_OS ANDROID)
SET(CPPBUILD_TARGET_ARCH AARCH64)
SET(CPPBUILD_TARGET_COMPILER_VERSION )
SET(CPPBUILD_TARGET_CPU_TYPE clang-r18b)
SET(CPPBUILD_TARGET_OS_VERSION )
SET(CPPBUILD_TARGET_CUDA_VERSION )
SET(CPPBUILD_TARGET_CUDNN_VERSION )
SET(CPPBUILD_TARGET_OCL_VERSION 2.0)
SET(CPPBUILD_TARGET_SNPE_VERSION 1.31)
SET(CPPBUILD_TARGET_NNIE_VERSION )
SET(CPPBUILD_TARGET_CVFLOW_VERSION )
SET(CPPBUILD_TARGET_COMPILER_VERSION )
SET(CPPBUILD_TARGET_TENSORRT_VERSION )
SET(CPPBUILD_TARGET_OPENCV_VERSION )
SET(CPPBUILD_TARGET_PYTHON_VERSION )


