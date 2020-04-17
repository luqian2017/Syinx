#pragma once
#include "../Syinx/SyTaskAdapter.h"
class IChannel;
class CPlayer :public IChannel
{
public:
	CPlayer(IChannel* inIch);
	~CPlayer();
	/*
	* ��������ʼʱ�����һ��
	* The server is called once to begin with
	*/
	bool Initialize()override;

	/*
	* �ͻ����ڳɹ�����ʱ�ᱻ����
	* The client is called upon a successful connection
	*/
	void Login() override;

	/*
	* �ͻ��˹ر�����ʱ�ᱻ����
	* When the client closes the connection, it is called
	*/
	void Logout()override;

	/*
	* �������ڱ��ر�ʱ�ᱻ����
	* The server is called when it is shut down
	*/
	void Close()override;


	/*
	* �ͻ��˷�����ʱ�¼�ʱ�ᱻ����
	* The client is called when a timeout event occurs
	*/
	void TimeOut()override;
private:
	IChannel* m_ICh;
};

