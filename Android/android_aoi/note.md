## 德赛车机

dtach -a /tmp/android
setprop persist.sv.debug.adb_enable 1

## 进入工程模式

adb root
adb remount
adb shell am start -n com.desaysv.engmode/.MainActivity

## 进入QNX前的步骤

pfctl -d
iptables -F
iptables -X
iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT

## 进入QNX

busybox telnet 192.168.8.1
root sv2655888

## 共享目录

android /share  qnx /var/share

## QNX打开写权限

mount -uw /

cd /data/BYDD9_DMSb080f171_231205.1424_linux-aarch64-qnx_231205;cp /var/share/byd_test ./;chmod 777 byd_test;
export PATH=$(pwd)/data/samples/tools:$(pwd)/data/samples/bin:$PATH;
export LD_LIBRARY_PATH=$(pwd)/lib:$(pwd)/samples/tools/opencv/lib;
export ADSP_LIBRARY_PATH=$(pwd)/lib;
export CDSP_LIBRARY_PATH=$(pwd)/lib;
./byd_test

#### android端打开ftp server

busybox tcpsvd 0 21 busybox ftpd -w /data

#### qnx登录方式

##### 先进入安卓

adb shell

##### 从安卓进入QNX

busybox telnet 192.168.8.1

用户名： root  密码： sv2655888

##### QNX从android端拉取文件

ftp 192.168.8.2
get
/share/dmsServer
/data/BYDD9_DMSb080f171_231205.1424_linux-aarch64-qnx_231205/dmsServer

## 一键进入QNX

./login_qnx.sh
