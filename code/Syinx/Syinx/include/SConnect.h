#pragma once
#include "GameObject.h"
class SyinxKernel;
class SConnect :public SObject
{
	friend class SyinxKernel;
	friend void ConnectInput(struct bufferevent* bev, void* ctx);
	enum SCONNECT_LINK_STATUS
	{
		CLIENT_LOGOUT = 1,
		CLIENT_LOGIN  = 2,
	};
private:
	evbuffer*		m_Inputevbuffer;
	bufferevent*	m_buffer;
	string			m_tag;
	int				m_Status;
public:
	SConnect();
	~SConnect();
	/*
	* 服务器连接其他服务器时将会调用
	*/
	static SConnect* Connect(string IP, short Port, string tag);


	/*
	* 被连接的服务器断开时会被调用
	*/
	void DisConnect();

	/*
	*该服务器被关闭是将会调用
	*/
	void Close()override;
	/*
	* 超时事件发生时会被调用
	*/
	void TimeOut();

	/*
	*当连接成功时会被调用
	*/
	void Finish();
private:
	bool Initialize()override;

	void OnStatusDoAction();

};

