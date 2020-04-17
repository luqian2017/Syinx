#pragma once
#include "../Syinx/include/SyTaskAdapter.h"
class IChannel;
class CPlayer :public IChannel
{
public:
	CPlayer(IChannel* inIch);
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
	IChannel* m_ICh;
};

