### 编译dmsServer

./scripts/build.sh

### 运行

将编译产物dmsServer放到dms_algo目录下，然后将dms_algo推入到QNX上.

进入到QNX:
cd dms_algo
export PATH=$(pwd)/data/samples/tools:$(pwd)/data/samples/bin:$PATH
export LD_LIBRARY_PATH=$(pwd)/lib:$(pwd)/samples/tools/opencv/lib
export CDSP_LIBRARY_PATH=$(pwd)/lib

执行
./dmsServer
