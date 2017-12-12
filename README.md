# ZeroNet

开发工具：Qt 5 MinGW<br>
服务端：Qt库<br>
客户端：Win32 API<br>
原作者：sumkee911<br>


ZeroNet 属于远程控制类软件，通过添加功能模块和完善代码逻辑，学习网络攻防和操作系统的相关知识，切勿作为黑客软件使用！！！<br>
使用该此程序进行网络攻击所造成的的后果由使用者承担！！！<br>

**安全性：**<br>
ZeroNet的客户端在原代码的基础上添加了打开确认机制，防止被人利用作为网络攻击工具<br>
若不小心点开了客户端且确认被控，手动清除方法：<br>
>1.删除此文件<br>
>>2.删除D盘根目录下的同名文件<br>
>>>3.删除注册表的开机自启动项       HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run\<br>
>>>>4.任务管理器中将同名进程结束<br>
>>>>>5.重启电脑<br>


**功能更新概览：**<br>

2017.12.11  ZeroNet Sever 2.0<br>
1.修改界面图标，为扁平化风格优化，去除客户端列表的滚动显示条<br>
3.客户信息增为  id username ip port system sity<br>
2.增加DDOS模块（无界面，点击启动类型） ，用于测试服务器压力<br>
3.增加作者信息，优化程序执行逻辑<br>
4.增加大量功能的错误提示信息，方便后续调试修改<br>

![界面UI](https://github.com/joliph/ZeroNet/blob/master/ZeroNet2.jpg)<br>
