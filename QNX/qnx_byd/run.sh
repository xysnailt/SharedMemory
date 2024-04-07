adb root
adb remount
adb shell am start -n com.desaysv.engmode/.MainActivity
adb shell setprop persist.sv.debug.adb_enable 1
adb shell iptables -F
adb shell iptables -X
adb shell iptables -P INPUT ACCEPT
adb shell iptables -P OUTPUT ACCEPT
adb shell iptables -P FORWARD ACCEPT
adb push ./build/dmsServer /share/
adb shell sync
md5sum ./build/dmsServer
adb shell md5sum /share/dmsServer