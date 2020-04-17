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
	//将用户的通道层挂在树上
	list<IChannel*>						m_listConnectClient;
	std::queue<IChannel*>				 m_queueILoop;
	size_t								 m_IChannelNum;
public:
	SyinxAdapterResource(int IChNum);
	~SyinxAdapterResource();
	bool Initialize();
	void Close();

	//将新连接的客户端进行分配
	bool AllocationIChannel(bufferevent* buffer, intptr_t _FD);	//分配一个IChannel

	bool SocketFdAdd(IChannel* ICh);							//将新的文件描述符做上树操作
	bool SocketFdDel(IChannel* ICh);							//将文件描述符下树

	void GameServerDoAction();									//处理所有帧
private:

};