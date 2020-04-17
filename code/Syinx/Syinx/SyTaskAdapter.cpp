#include "include/SyInc.h"
#include "include/Syinx.h"
#include "include/SyResAdapter.h"
#include "include/SyPthreadPool.h"
#include "include/SyTaskAdapter.h"
#include "include/Tools.hpp"
#include "../pkg/easylog/easylogging++.h"
#include "../Player/CPlayer.h"


IChannel::IChannel()
{
	m_IChannelID			= 0;
	IChStatus				= CLIENT_LOGOUT;
	m_buffer				= nullptr;
	m_Inputevbuffer			= nullptr;
	m_Socket				= 0;
	m_pPlayer				= nullptr;
	DoNotBindParse			= false;
}

IChannel::~IChannel()
{
	m_IChannelID			= 0;
	IChStatus				= CLIENT_LOGOUT;
	m_buffer				= nullptr;
	m_Inputevbuffer			= nullptr;
	m_pPlayer				= nullptr;
	m_Socket				= 0;
	DoNotBindParse			= false;
}

void* PthreadFuncHandle(void* arg)
{
	IChannel* Ich = (IChannel*)arg;
	if (Ich == nullptr)
	{
		LOG(ERROR) << "IChannel is NULL";
		return NULL;
	}
	Ich->OnStatusDoAcionInPthread();
	return NULL;
}

bool IChannel::RecvValuesFromString(char* _Instr, int _InSize, int& Index, int& _OutLen, int& _OutType, char* _OutStr)
{
	memcpy(_OutStr, _Instr, _InSize);
	_OutLen = _InSize;
	_OutType = 1;
	Index = 0;
	return false;
}

bool IChannel::RecvClientPack(int& _InLen, int& _InType, const char* _InStr)
{
	//LOG(INFO) << "Recv Pack";
	//LOG(INFO) << "Recv Data: " << _InStr<<"  Recv Len:  "<< _InLen;
	SendDataToClient( _InStr, _InLen);
	return true;
}



bool IChannel::SendValuesToString(uint32_t _InLen, uint32_t _InType, std::string& _InStr)
{
	std::string _SendStr;
	_SendStr.reserve(_InLen + 8);
	int _Num = 0;
	if (_InLen > 0)
	{
		_SendStr.push_back(_InLen & 0xff);
		_SendStr.push_back((_InLen >> 8) & 0xff);
		_SendStr.push_back((_InLen >> 16) & 0xff);
		_SendStr.push_back((_InLen >> 24) & 0xff);

		//type
		_SendStr.push_back(_InType & 0xff);
		_SendStr.push_back((_InType >> 8) & 0xff);
		_SendStr.push_back((_InType >> 16) & 0xff);
		_SendStr.push_back((_InType >> 24) & 0xff);

		_SendStr.append(_InStr);
		int iRet = bufferevent_write(m_buffer, _SendStr.c_str(), _SendStr.size());
		if (iRet < 0)
		{
			return false;
		}
		return true;
	}
	else
		return false;
}

int IChannel::SendDataToClient(const char* data, size_t packsize)
{
	if (IChStatus == CLIENT_LOGIN)
	{
		return bufferevent_write(m_buffer, data, packsize);
	}
	return -1;
}

void IChannel::OnStatusDoAcionInPthread()
{
	evbuffer_lock(m_Inputevbuffer);
	char strData[BUFFSIZE] = { 0 };
	int buffersize = evbuffer_remove(m_Inputevbuffer, strData, BUFFSIZE);
	if (buffersize <= 0)
		return;
	int Index = 0;
	do
	{
		int PackLen = 0, PackType = 0;

		char OutStr[BUFFSIZE] = { 0 };

		if ((this->*ParseFunc)(strData, buffersize, Index, PackLen, PackType, OutStr))
		{
			RecvClientPack(PackLen, PackType, OutStr);
		}
		else
		{
			RecvClientPack(PackLen, PackType, OutStr);
			break;
		}
	} while (true);
	evbuffer_unlock(m_Inputevbuffer);
}

void IChannel::SetupFrameInputFunc()
{
	BindParse(&IChannel::RecvValuesFromString);
}


inline bool IChannel::GetWhetherBindParseFunc()
{
	return DoNotBindParse;
}

bool IChannel::Initialize()
{
	SetupFrameInputFunc();
	if (!GetWhetherBindParseFunc())
	{
		//Ã»ÓÐ°ó¶¨
		LOG(ERROR) << "Not Bind Parse";
		return false;
	}

	if (!m_pPlayer)
	{
		m_pPlayer = CreateObject<CPlayer>::Instance(this);
		if (!m_pPlayer->Initialize())
		{
			LOG(ERROR) << "CPlayer Initialize Failed";
			return false;
		}
	}
	if (m_Inputevbuffer == NULL)
	{
		m_Inputevbuffer = evbuffer_new();
	}
#ifdef SYINX_USE_PTHREAD
	evbuffer_enable_locking(m_Inputevbuffer, NULL);
#endif
	return true;
}

void IChannel::Login()
{
}


void IChannel::Logout()
{

}

void IChannel::Close()
{
	if (m_pPlayer)
	{
		m_pPlayer->Close();
		delete m_pPlayer;
		m_pPlayer = nullptr;
	}
	if (m_Inputevbuffer)
	{
		evbuffer_free(m_Inputevbuffer);
	}
}

void IChannel::TimeOut()
{
}

void IChannel::OnClientConnect(intptr_t _fd, bufferevent* _buffer, int _id)
{
	m_Socket = _fd;
	if (nullptr != _buffer)
	{
		m_buffer = _buffer;
	}
	m_pPlayer->Login();
	IChStatus = CLIENT_LOGIN;
}

void IChannel::OnStatusDoAction()
{
	if (!evbuffer_get_length(m_Inputevbuffer))
		return;
#ifdef SYINX_USE_PTHREAD
	PTHREADFUNCADD(PthreadFuncHandle, this);
#else
	char strData[BUFFSIZE] = { 0 };

	int buffersize = evbuffer_remove(m_Inputevbuffer, strData, BUFFSIZE);

	int Index = 0;
	do
	{
		int PackLen = 0, PackType = 0;

		char OutStr[BUFFSIZE] = { 0 };

		if ((this->*ParseFunc)(strData, buffersize, Index, PackLen, PackType, OutStr))
		{
			RecvClientPack(PackLen, PackType, OutStr);
		}
		else
		{
			RecvClientPack(PackLen, PackType, OutStr);
			break;
		}
	} while (true);
#endif
}

void IChannel::Clear()
{
	m_Socket = 0;
	OnStatusDoAction();
	IChStatus = CLIENT_LOGOUT;
	if (nullptr !=m_buffer)
	{
		bufferevent_free(m_buffer);
		m_buffer = nullptr;
	}
	m_pPlayer->Logout();
}


intptr_t IChannel::GetSocket() const
{
	return m_Socket;
}

bufferevent* IChannel::GetBuffer()
{
	return m_buffer != nullptr ? m_buffer : nullptr;
}

const size_t IChannel::GetIChannelUniqueID()
{
	return m_IChannelID;
}

void IChannel::SetIChannelID(size_t index)
{
	if (!m_IChannelID)
	{
		m_IChannelID = index;
	}
}

CPlayer* IChannel::GetCPlayer()
{
	return m_pPlayer;
}

void IChannel::BindParse(bool(IChannel::* RecvBufferParse)(char* _Instr, int _InSize, int& PtrIndex, int& _OutLen, int& _OutType, char* _OutStr))
{
	if (RecvBufferParse != NULL)
	{
		ParseFunc = RecvBufferParse;
		DoNotBindParse = true;
		return;
	}
	DoNotBindParse = false;
	return;
}


