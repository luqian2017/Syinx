#pragma once
#include "GameObject.h"
class SConnect :public SObject
{
private:
	evbuffer*		m_Inputevbuffer;
	bufferevent*	m_buffer;
	string			m_tag;
public:
	SConnect();
	~SConnect();
	/*
	* ��������������������ʱ�������
	*/
	bool Connect(string IP, short Port, string tag);

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

