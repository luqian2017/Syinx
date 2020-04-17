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
	* ��ǰICh�������һ��Socket�׽���
	*/
	intptr_t	 m_Socket;

	/*
	* ͨ��Ψһid
	*/
	size_t m_IChannelID;

	/*
	* ���ͻ��˷����һ��libevent  bufferevent 
	*/
	bufferevent*		 m_buffer;

	/*
	* �ͻ��˵�evbuffer����
	*/
	evbuffer*			m_Inputevbuffer;

	/*
	* ��ҵ�ָ��
	*/
	CPlayer* m_pPlayer;


	typedef bool (IChannel::*RecvBufferParse)(char* _Instr, int _InSize, int& PtrIndex,int& _OutLen, int& _OutType, char* _OutStr);
	RecvBufferParse ParseFunc;

	bool DoNotBindParse;
	
public:
	IChannel();
	~IChannel();
	/*
	*��ʼ��һ��ͨ����
	*�ٴ����ð󶨵�player class Player �����̳�
	*Initializes a channel layer
	*/
	virtual bool Initialize();

	/*
	*�ͻ��˵�½ʱPlayerĬ�ϵ���Login����
	*The Login method is invoked by default by the Player when the client logs in
	*/
	virtual void Login();

	/*
	*�ͻ����˳���PlayerĬ�ϵ���Logout����
	*
	*/
	virtual void Logout();

	/*
	*������˳�ʱ�ر����ͨ����
	*/
	virtual void Close();

	virtual void TimeOut();

	/*
	* ��ȡͨ����Ψһid,Ψһid�ڸ�����Ϊ˽�г�Ա,�޷��̳�,�й��캯����ʼ��Ϊ0
	* ���ø���ָ���ȡ
	*/
	const size_t GetIChannelUniqueID();
private:
	/*
	* 
	*/
	void SetIChannelID(size_t index);
	/*
	*���ͻ�������ʱ����һ��buffer
	*/
	void  OnClientConnect(intptr_t _fd, bufferevent* buffer, int m_ClientID = 0);

	/*
	* �ڵ��̻߳����ڶ��̲߳����¶�ȡ���߼�����
	*/
	void OnStatusDoAction();
	/*
	*�ڶ��߳�������Ϸ�߼�����
	*���Ǳ���ָ�� SYINX_USE_PTHREAD ��
	*/
	void OnStatusDoAcionInPthread();

	/*
	*	������߿��,�󶨽��յ����߼�֡����εĽ��� 
	*
	*/
	void SetupFrameInputFunc();

	/*
	* �鿴�Ƿ��û����˽�������
	*/
	bool GetWhetherBindParseFunc();

	/*
	* ���ܽ�������pack
	*/
	bool RecvClientPack(int& _InLen, int& _InType, const char* _InStr);


	//Э�����
	bool RecvValuesFromString(char* _Instr, int _InSize, int& Index, int& _OutLen, int& _OutType, char* _OutStr);


	bool SendValuesToString(uint32_t _InLen, uint32_t _InType, std::string& _InStr);


	int SendDataToClient( const char* data, size_t packsize);

	/*
	* ���ͻ����˳�,���������ǰ�ͻ��˻���
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
	�󶨴���Ļ��߽����ĺ�����Ҫ�û��Լ��������ȥ����
	bool RecvBufferParse(char* _Instr, int _InSize, int& PtrIndex, int& _OutLen, int& _OutType, char* _OutStr);
	_InStr��һ�������ԭʼ�ַ���
	_InSize�Ǵ����ԭʼ�ַ�������
	PtrIndex��һ���±������,TCP��֡������������,���ܻᷢ��ճ������,���Ը�ֵ����ȷ��ÿ�����Ĳ�ֺ��λ��
	_OutLen�ǽ�����󴫳��ĳ���
	_OutType�ǽ�����󴫳�������
	_OutStr�ǽ�����󴫳���IO��

	*/
	void BindParse(bool (IChannel::*RecvBufferParse)(char* _Instr, int _InSize, int& PtrIndex, int& _OutLen, int& _OutType, char* _OutStr));
};

#endif
