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
	* ��������������������ʱ�������
	*/
	static SConnect* Connect(string IP, short Port, string tag);


	/*
	* �����ӵķ������Ͽ�ʱ�ᱻ����
	*/
	void DisConnect();

	/*
	*�÷��������ر��ǽ������
	*/
	void Close()override;
	/*
	* ��ʱ�¼�����ʱ�ᱻ����
	*/
	void TimeOut();

	/*
	*�����ӳɹ�ʱ�ᱻ����
	*/
	void Finish();
private:
	bool Initialize()override;

	void OnStatusDoAction();

};

