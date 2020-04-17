#include "SyInc.h"
#include "SConnect.h"
#include "../Sylog/easylogging++.h"
#include "Syinx.h"
SConnect::SConnect()
{
	m_buffer = nullptr;
	m_Inputevbuffer = nullptr;
}

SConnect::~SConnect()
{

}

void ConnectInput(struct bufferevent* bev, void* ctx)
{

}

void ConnectEvent(struct bufferevent* bev, short what, void* ctx)
{
	if (ctx == NULL)
		return;
	if (what & BEV_EVENT_CONNECTED)
	{
		((SConnect*)ctx)->Finish();
	}
	else if (what & BEV_EVENT_ERROR)
	{
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
	return;

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

bool SConnect::Connect(string IP, short Port, string tag)
{
	if (!Initialize())
	{
		return false;
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
	m_buffer = bufferevent_socket_new(g_pSyinx.GetEventBase(), -1, BEV_OPT_CLOSE_ON_FREE);
	if (m_buffer == NULL)
	{
		LOG(ERROR) << "bufferevent is null";
		return false;
	}
	if (bufferevent_socket_connect(m_buffer, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		LOG(ERROR) << "Connect " << IP << "  " << Port << "Failed";
		return false;
	}
	bufferevent_setcb(m_buffer, ConnectInput, NULL, ConnectEvent, this);

	bufferevent_enable(m_buffer, EV_READ| EV_PERSIST);

	bufferevent_setwatermark(m_buffer, EV_READ, LIBEVENT_READ_WATERMARK, 0);
	m_tag = tag;
	
	return true;
}

void SConnect::DisConnect()
{
	LOG(INFO) << m_tag << " DisConnect";
	SyinxKernel::RemoveConnect(this);
}

void SConnect::Close()
{

}

void SConnect::TimeOut()
{
}

void SConnect::Finish()
{
	LOG(INFO) << m_tag << " is Connect";
	SyinxKernel::ListenerConnect(this);
}

void SConnect::OnStatusDoAction()
{
}
