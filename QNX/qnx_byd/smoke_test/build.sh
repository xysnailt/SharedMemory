if [ -e build ]; then rm -rf build; fi

source /usr/local/toolchains/qnx700/qnxsdp-env.sh
mkdir build && cd build && cmake .. -DTARGET_CPU:STRING=qnx -DCMAKE_TOOLCHAIN_FILE=/usr/local/toolchains/cppbuild-1.0.0/assets/cmake/toolchain/linux-aarch64-qnx.cmake -DCPPBUILD_TARGET_CPU_TYPE:STRING=qnx -DTEST_IN_SINGLE_FOLDER_IN_QNX=ON && make VERBOSE=1 -j8 && cd ..
