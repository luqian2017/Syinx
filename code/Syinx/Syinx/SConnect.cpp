#include "include/SyInc.h"
#include "include/Syinx.h"
#include "include/SConnect.h"
#include "include/Tools.hpp"
#include "../pkg/easylog/easylogging++.h"
SConnect::SConnect()
{
	m_buffer = nullptr;
	m_Inputevbuffer = nullptr;
	m_Status=0;
	m_tag.clear();
}

SConnect::~SConnect()
{
	m_buffer = nullptr;
	m_Inputevbuffer = nullptr;
	m_Status = 0;
	m_tag.clear();
}

void ConnectInput(struct bufferevent* bev, void* ctx)
{
	if (ctx == nullptr)
		return;
	size_t iRet = bufferevent_read_buffer(bev, ((SConnect*)ctx)->m_Inputevbuffer);
	if (iRet < 0)
		LOG(ERROR) << "Read buffer Failed Errno:" << iRet;
}

void ConnectEvent(struct bufferevent* bev, short what, void* ctx)
{
	if (ctx == nullptr)
		return;
	if (what & BEV_EVENT_CONNECTED)
	{
		((SConnect*)ctx)->Finish();
	}
	else if (what & BEV_EVENT_ERROR)
	{
		int Error = evutil_socket_geterror(bufferevent_getfd(bev));
		LOG(ERROR) << "Some error Happend erron:[ " << Error << " ] Describe:" << evutil_socket_error_to_string(Error);
		((SConnect*)ctx)->DisConnect();
	}
	else if (what & BEV_EVENT_TIMEOUT)
	{
		((SConnect*)ctx)->TimeOut();
	}
	else if (what & BEV_EVENT_EOF)
	{
		((SConnect*)ctx)->DisConnect();
	}
	else if (what & BEV_EVENT_WRITING)
	{
		LOG(ERROR) << "Writting Error";
	}
	else if (what & BEV_EVENT_READING)
	{
		LOG(ERROR) << "Reading Error";
	}
}
bool SConnect::Initialize()
{
	if (m_Inputevbuffer == NULL)
	{
		m_Inputevbuffer = evbuffer_new();
		if (m_Inputevbuffer == NULL)
			return false;
		else
			return true;
	}
	return true;
}

SConnect* SConnect::Connect(string IP, short Port, string tag)
{
	SConnect* NewServer = CreateObject<SConnect>::Instance();
	if (NewServer == nullptr)
	{
		return nullptr;
	}
	do 
	{
		if (!NewServer->Initialize())
		{
			break;
		}

		struct sockaddr_in addr;

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(Port);
#ifdef WIN32
		addr.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
#else
		inet_pton(AF_INET, IP.c_str(), &addr.sin_addr);
#endif
		auto EventBase = g_pSyinx.GetEventBase();
		if (EventBase == nullptr)
		{
			LOG(ERROR)<<"GetEventBase is nullptr";
			break;
		}
		NewServer->m_buffer = bufferevent_socket_new(EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
		if (NewServer->m_buffer == nullptr)
		{
			break;
		}
		if (bufferevent_socket_connect(NewServer->m_buffer, (sockaddr*)&addr, sizeof(addr)) < 0)
		{
			LOG(ERROR) << "Connect " << IP << "  " << Port << "Failed";
			break;
		}
		bufferevent_setcb(NewServer->m_buffer, ConnectInput, NULL, ConnectEvent, NewServer);

		bufferevent_enable(NewServer->m_buffer, EV_READ | EV_PERSIST);

		bufferevent_setwatermark(NewServer->m_buffer, EV_READ, LIBEVENT_READ_WATERMARK, 0);
		NewServer->m_tag = tag;

		return NewServer;
	} while (0);
	if (NewServer != nullptr)
	{
		delete NewServer;
		return nullptr;
	}
	return nullptr;
	
}

void SConnect::DisConnect()
{
	LOG(INFO) << m_tag << " DisConnect";
	m_Status = CLIENT_LOGOUT;
	SyinxKernel::RemoveConnect(this);
}

void SConnect::Close()
{
	if (m_buffer)
	{
		bufferevent_free(m_buffer);
	}
	if (m_Inputevbuffer)
	{
		evbuffer_free(m_Inputevbuffer);
	}
}

void SConnect::TimeOut()
{
}

void SConnect::Finish()
{
	LOG(INFO) << m_tag << " is Connect Success";
	m_Status = CLIENT_LOGIN;
	SyinxKernel::ListenerConnect(this);
}

void SConnect::OnStatusDoAction()
{

}
