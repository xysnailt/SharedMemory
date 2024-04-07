#---------------------------------------------------------------
# CPPBUILD ToolChain related Settings.
# Note: include this fine before 'project(...)' command in samples
#---------------------------------------------------------------
#set toolchains.
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER "/usr/local/toolchains/qnx700/host/linux/x86_64/usr/bin/aarch64-unknown-nto-qnx7.0.0-gcc")
SET(CMAKE_CXX_COMPILER "/usr/local/toolchains/qnx700/host/linux/x86_64/usr/bin/aarch64-unknown-nto-qnx7.0.0-g++")

#currently we only support samples on linux, windows, android, and macos
SET(CPPBUILD_CXX_DIALECT c++11)
SET(CPPBUILD_TARGET_OS LINUX)

