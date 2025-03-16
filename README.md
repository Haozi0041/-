# 电子密码锁设计
这是一个基于51单片机的电子密码锁，可以实现基本密码锁功能。
设计一个电子密码锁。
    1、通过矩阵键盘设置密码；
    2、矩阵键盘输入密码在LCD1602屏幕上显示，密码正确则启动继电器实现开锁；
    3、密码不正确将提示重新输入，三次输入错误报警并锁定键盘3分钟；
    4、输入密码过程中可选择重新输入密码；
    5、断电保存数据；
    6、可以休眠和唤醒。
    7、可以进行密码屏蔽，提高安全性。
# 使用说明
开机后，可以使用4x4矩阵键盘作为输入设备，通过扫描键盘（0~9）获取用户输入的密码。若想删除已输入的密码，可以按下11号按键，进行逐个删除。

在输入密码后，按下10号按键进行确认。此时系统会对密码进行确认，若正确，提示”passed”,同时继电器打开。若错误，则会提示“wrong password”。连续三次输入错误后，系统会触发报警（蜂鸣器），并锁定键盘3分钟。

在正确输入密码后，按下12号按键进入设置模式，用户输入新密码后保存到EEPROM中。（只有在正确输入情况下可以进行密码设置）。
锁定期间，键盘输入无效，LCD显示“Locking”。

系统在无操作一段时间后进入休眠模式，以降低功耗。休眠模式下，LCD屏幕关闭，通过按下任意键唤醒系统，恢复正常工作状态。

在正确输入后按下13按键会重新上锁。按下15按键会强制进行休眠。
若在输入模式下按下14号按键，则会屏蔽已经输入的密码，统一替换为 “*”。

# 注意事项
在使用本项目前，请确认电路连接状态。
矩阵键盘的8个IO口连接到单片机P0口。

本次显示功能采用LCD1602显示屏。将数据传输口连接到单片机P1口,同时将进行控的三个端口RS  RW  EN 连接到P3.5   P3.6   P3.7口

使用AT24C02模块进行数据储存，其可以确保数据能够掉电不丢失。其SCL SDA分别连接到P2.1  P2.2口。

继电器控制口连接到P2.0  蜂鸣器连接到P2.3口。
