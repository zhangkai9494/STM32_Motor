﻿# STM32_Motor
作品提交库

2019年7月31日23:10:40 版本号：v0.0
已知BUG：
   角度是减小的
   偶尔速度会和设定值误差较大
   设定角度未实现
   按键14按下不管用
下一步：
   通过按键切换控制模式
   设定角度可以旋转

2019年8月1日15:14:52 版本号：v0.1
修复BUG：
   角度问题解决
已知BUG：
   按键有时候失灵
   按键14仍然不管用
新功能：
   加入模式转换功能
   EEPROM开辟三个内存块用来储存新的PID参数

2019年8月2日13:02:35 版本号：v0.2
已知BUG：
   串口指令接收有问题
   按键14仍然不管用
更改：
   将串口菜单放入usart.c中执行

2019年8月10日22:03:28 版本号：v1.0
已知BUG：
   前几位显示正常 调节系数的后两位显示不正常为 0.00099和0.00009
更改：
   将UI.c中的sprintf函数删除，改用自己编写的 float_to_char函数 
   将串口菜单放入主函数执行
动态：
   github版本控制开始使用 
下一步：
   完善代码注释（标准化）
   float_to_char的修改 
   控制转过角度实现 
   通过串口接收指令

2019年8月11日14:08:52 版本号：v1.1
修复BUG:
   float_to_char函数没有问题 错误未知

2019年8月12日11:42:38 版本号：v1.2
修复BUG： 
   数字判定有问题 加入新指令解决 
已知BUG：
   调节系数后两位仍然显示不正常
新功能：
   串口指令部分完成

2019年8月13日11:04:54 版本号：v1.3
修改：
   优化程序结构
   增加添加代码注释
   调整速度PID参数为 P0.46499 I0.00010 D0.42610 角度PID参数为 P1.34309 I0.00000 D0.63900  
已知BUG：
   按键最后一排经常失灵
   调节系数后两位仍然显示不正常
   调整系数有时候加减操作不正常
   转过角度无法一次性到位，并且误差较大