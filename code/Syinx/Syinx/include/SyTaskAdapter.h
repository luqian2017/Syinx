#ifndef _SYTASKADAPTER_H_
#define _SYTASKADAPTER_H_

#include "GameObject.h"
#include <stdint.h>
#include <iostream>

class SyinxLog;
class SyinxAdapterResource;
class IChannel;
class CPlayer;
class SyinxKernel;
struct evbuffer;
struct bufferevent;

class IChannel:public STask
{
	friend void* PthreadFuncHandle(void* arg);
	friend void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx);
	friend class SyinxAdapterResource;
	friend class CPlayer;
	enum ICNANNEL_LINK_STATUS
	{
		CLIENT_LOGOUT = 1,
		CLIENT_LOGIN = 2,
	};
private:
	/*
	* 当前ICh被分配的一个Socket套接字
	*/
	intptr_t	 m_Socket;

	/*
	* 通道唯一id
	*/
	size_t m_IChannelID;

	/*
	* 给客户端分配的一个libevent  bufferevent 
	*/
	bufferevent*		 m_buffer;

	/*
	* 客户端的evbuffer缓存
	*/
	evbuffer*			m_Inputevbuffer;

	/*
	* 玩家的指针
	*/
	CPlayer* m_pPlayer;


	typedef bool (IChannel::*RecvBufferParse)(char* _Instr, int _InSize, int& PtrIndex,int& _OutLen, int& _OutType, char* _OutStr);
	RecvBufferParse ParseFunc;

	bool DoNotBindParse;
	
public:
	IChannel();
	~IChannel();
	/*
	*初始化一个通道层
	*再此设置绑定的player class Player 类必须继承
	*Initializes a channel layer
	*/
	virtual bool Initialize();

	/*
	*客户端登陆时Player默认调用Login方法
	*The Login method is invoked by default by the Player when the client logs in
	*/
	virtual void Login();

	/*
	*客户端退出是Player默认调用Logout方法
	*
	*/
	virtual void Logout();

	/*
	*框架在退出时关闭这个通道层
	*/
	virtual void Close();

	virtual void TimeOut();

	/*
	* 获取通道的唯一id,唯一id在父类中为私有成员,无法继承,切构造函数初始化为0
	* 利用父类指针获取
	*/
	const size_t GetIChannelUniqueID();
private:
	/*
	* 
	*/
	void SetIChannelID(size_t index);
	/*
	*当客户端连接时分配一个buffer
	*/
	void  OnClientConnect(intptr_t _fd, bufferevent* buffer, int m_ClientID = 0);

	/*
	* 在单线程或者在多线程并发下读取做逻辑处理
	*/
	void OnStatusDoAction();
	/*
	*在多线程中做游戏逻辑处理
	*但是必须指定 SYINX_USE_PTHREAD 宏
	*/
	void OnStatusDoAcionInPthread();

	/*
	*	必须告诉框架,绑定接收到的逻辑帧做如何的解析 
	*
	*/
	void SetupFrameInputFunc();

	/*
	* 查看是否用户绑定了解析函数
	*/
	bool GetWhetherBindParseFunc();

	/*
	* 接受解析完后的pack
	*/
	bool RecvClientPack(int& _InLen, int& _InType, const char* _InStr);


	//协议解析
	bool RecvValuesFromString(char* _Instr, int _InSize, int& Index, int& _OutLen, int& _OutType, char* _OutStr);


	bool SendValuesToString(uint32_t _InLen, uint32_t _InType, std::string& _InStr);


	int SendDataToClient( const char* data, size_t packsize);

	/*
	* 当客户端退出,用来清除当前客户端缓存
	*/
	void Clear();

public:
	
	intptr_t			GetSocket()const;
	bufferevent*		GetBuffer();
	CPlayer*			GetCPlayer();
	int					IChStatus;
private:
	/*
	*
	绑定处理的或者解析的函数需要用户自己定义如何去解析
	bool RecvBufferParse(char* _Instr, int _InSize, int& PtrIndex, int& _OutLen, int& _OutType, char* _OutStr);
	_InStr是一个传入的原始字符串
	_InSize是传入的原始字符串长度
	PtrIndex是一个下标的引用,TCP的帧传递是连续的,可能会发生粘包现象,所以该值用来确定每个包的拆分后的位置
	_OutLen是解析完后传出的长度
	_OutType是解析完后传出的类型
	_OutStr是解析完后传出的IO包

	*/
	void BindParse(bool (IChannel::*RecvBufferParse)(char* _Instr, int _InSize, int& PtrIndex, int& _OutLen, int& _OutType, char* _OutStr));
};

#endif
