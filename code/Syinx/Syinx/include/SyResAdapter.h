#pragma once

#include "GameObject.h"
#include <unordered_map>
#include <queue>
#include <list>

class SyinxKernel;
class SyinxAdapter;
class SyinxAdapterPth;
class SyinxLog;
class IChannel;
class CPlayer;

#define SOCKETS	                                         int
#define BUFFSIZE										 4096

class SyinxAdapterResource : public SObject
{
	friend void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx);
	friend class SyinxKernel;
private:
	//���û���ͨ�����������
	list<IChannel*>						m_listConnectClient;
	std::queue<IChannel*>				 m_queueILoop;
	size_t								 m_IChannelNum;
public:
	SyinxAdapterResource(int IChNum);
	~SyinxAdapterResource();
	bool Initialize();
	void Close();

	//�������ӵĿͻ��˽��з���
	bool AllocationIChannel(bufferevent* buffer, intptr_t _FD);	//����һ��IChannel

	bool SocketFdAdd(IChannel* ICh);							//���µ��ļ�����������������
	bool SocketFdDel(IChannel* ICh);							//���ļ�����������

	void GameServerDoAction();									//��������֡
private:

};