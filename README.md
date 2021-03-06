

![miku](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/miku.jpeg)

![Syinx](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/Syinx.png)

------

[![Syinx](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/goto.svg)](https://www.jianshu.com/u/3f6b82b7329d)

[![Syinx](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/bhu.svg)](https://www.zhihu.com/people/lee-78-27-78/activities)

​	基于libevent所应用的跨平台适用于游戏的服务器,可以根据不用的需求进行多线程高并发或者单线程模型进行开发

The cross-platform application based on libevent is suitable for game servers, which can be developed according to different requirements of multi-threaded high concurrency or single-threaded model

> 版本:
>
> C/C++:0.7.0
>
> 适用平台:WIndows,Linux
>
> 开发: SiCen Li

# 如何测试(how to test?)

### Linux:

```
$ cd code/Syinx/
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug/Release
$ make
```

### Windows:

```
$ cd code/Syinx/
$ cmake . -DCMAKE_BUILD_TYPE=Debug/Release
```

#### Windows中则创建vs工程文件



# 依赖库(Dependent libraries)

**libevent :安装与访问**

`sudo apt-get install libevent`

[Libevent]:https://github.com/libevent/libevent

**pthread:安装与访问**

`sudo apt-get install glibc-doc`

`sudo apt-get install manpages-posix-dev`

[Pthread]:https://computing.llnl.gov/tutorials/pthreads/



框架使用了第三方HerdOnly库

感谢作者的优秀的工具,方便他人开发

easyloggingpp:

`https://github.com/amrayn/easyloggingpp`

inih:

`https://github.com/benhoyt/inih`





# 帮助(Help)

```c++
#include "Syinx/include/SyInc.h"
#include "Syinx/include/Syinx.h"

int main(int argc, char* argv[])
{
	if (!g_pSyinx.Config(argc, argv))
		return -1;
	if (!g_pSyinx.Initialize())
		return -1;

	g_pSyinx.Run();

	g_pSyinx.Close();
}
```



在CPlayer中已经为用户重写好了一些成员方法,他们都会在特殊时候被调用

```c++
#pragma once
#include "../Syinx/SyTaskAdapter.h"
class CPlayer :public IChannel
{
public:
	CPlayer(IChannel& inIch);
	~CPlayer();
	/*
	* 服务器开始时会调用一次
	* The server is called once to begin with
	*/
	bool Initialize()override;

	/*
	* 客户端在成功连接时会被调用
	* The client is called upon a successful connection
	*/
	void Login() override;

	/*
	* 客户端关闭连接时会被调用
	* When the client closes the connection, it is called
	*/
	void Logout()override;

	/*
	* 服务器在被关闭时会被调用
	* The server is called when it is shut down
	*/
	void Close()override;


	/*
	* 客户端发生超时事件时会被调用
	* The client is called when a timeout event occurs
	*/
	void TimeOut()override;
private:
	IChannel& m_ICh;
};
```





```c++
在IChannel中 SetupFrameInputFunc函数将会指定一个解析函数的函数指针
void SetupFrameInputFunc();

在该函数中调用BindParse函数将会绑定一个你自己写的解析函数,该函数必须解析客户端发来的包
    
    
绑定处理的或者解析的函数需要用户自己定义如何去解析
@ _InStr是一个传入的原始字符串
@ _InSize是传入的原始字符串长度
@ PtrIndex是一个下标的引用,TCP的帧传递是连续的,可能会发生粘包现象,所以该值用来确定每个包的拆分后的位置
@ _OutLen是解析完后传出的长度
@ _OutType是解析完后传出的类型
@ _OutStr是解析完后传出的IO包    
void BindParse(bool (IChannel::*RecvBufferParse)(char* _Instr, int _InSize, int& PtrIndex, int& _OutLen, int& _OutType, char* _OutStr));

在帧处理函数中,将会调用你的解析函数,来进行粘包分割,Index用来控制字符串的下标位置,如果分割完毕后返回false
RecvClientPack将至少会被调用一次
int Index = 0;
do
{
		int PackLen = 0, PackType = 0;

		char OutStr[BUFFSIZE] = { 0 };

		if ((this->*ParseFunc)(strData, buffersize, Index, PackLen, PackType, OutStr))
		{
			RecvClientPack(PackLen, PackType, OutStr);
		}
		else
		{
			RecvClientPack(PackLen, PackType, OutStr);
			break;
		}
} while (true);
```



Config.ini Help

```ini
[ServerConfig]

#设置端口号
#set server port
Port			= 20001

#设置客户端连接最大数量
#set client connect max num
ConnectNum		= 10000

#是否为守护进程
#Whether for daemon
DaemonProcess	= false

#设置服务器模式 可选 (TCP,HTTP)  暂不支持UDP
#Set server mode (TCP,HTTP). Temporarily not supported UDP
SetModel		= tcp

#log 配置文件路径
LogConfigPath	= ./Syinx-Server.logger.conf 


[PthreadSetting]
#是否启用多线程模块
#Whether to enable multithreaded modules
UsePthreadPool	= yes

#设置线程开启数量
#Sets the number of threads to be opened
SetThreadNum	= 4

#设置任务队列数量
SetTaskNum		= 10000
```







