#!/usr/bin/expect
set timeout 10; # 设置超时时间

spawn adb shell busybox telnet 192.168.8.1; # 启动 Telnet 进程

expect "login:" ; # 等待登录提示
send "root\r" ; # 输入用户名，注意要包含回车符

expect "Password for root@localhost:" ; # 等待密码提示
send "sv2655888\r" ; # 输入密码，注意要包含回车符

# sleep 1

interact

send "exit\r" ; # 发送退出命令
expect eof ; # 等待进程结束