#include <iostream>
#include <cstring>
#include <map>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include "Syinx.h"
#include "SyAdapter.h"
#include "SyTaskAdapter.h"
#include "SyResAdapter.h"
#include "../Sylog/SyLog.h"
#ifdef SYINXMOD_ADD_MYSQL
#include <mysql/mysql.h>
#else
#endif 



int IChannel::RecvAllDataFromString(std::string& arg)
{
	if (this->StrByte->_InStr.size() <= 0 || this->StrByte->_InStr.size() != StrByte->_InSize)
	{
		SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "Buffer is empty : _Instr size 0");
		return -1;
	}
	int _Size = this->StrByte->_InStr.size();
	arg = this->StrByte->_InStr;

	this->StrByte->_InStr.erase(0, arg.size());
	this->StrByte->_InSize = 0;
	return _Size;
}

//按协议读取
int IChannel::RecvValuesFromString(unsigned int* _OutLen, unsigned int* _OutType, std::string& _OutStr)
{
	if (this->StrByte->_InStr.size() >= 8)
	{
		unsigned int ValuesLen =
			this->StrByte->_InStr[0] |
			this->StrByte->_InStr[1] << 8 |
			this->StrByte->_InStr[2] << 16 |
			this->StrByte->_InStr[3] << 24;

		unsigned int ValuesType =
			this->StrByte->_InStr[4] |
			this->StrByte->_InStr[5] << 8 |
			this->StrByte->_InStr[6] << 16 |
			this->StrByte->_InStr[7] << 24;

		_OutStr = StrByte->_InStr.substr(8, ValuesLen);
		*_OutLen = ValuesLen;
		*_OutType = ValuesType;

		this->StrByte->_InStr.erase(0, 8 + ValuesLen);
		this->StrByte->_InSize -= (8 + ValuesLen);
		return BufferSucc;
	}
	else if (StrByte->_InStr.size() <= 8)
	{
		StrByte->_InStr.clear();
		return BufferEnd;
	}
	else if (!StrByte->_InStr.size())
	{
		//如果接受的是个无数据包的包则返回err end
		return BufferNull;
	}
}

int IChannel::SendAllDataToString(std::string& _InStr)
{
	auto buffer = this->mICMsg->buffer;
	char* buf = const_cast<char*>(_InStr.c_str());
	if (strlen(buf) == _InStr.size())
	{
		int iRet = bufferevent_write(buffer, buf, strlen(buf));
		if (iRet < 0)
		{
			return 0;
		}
	}

	return strlen(buf);
}

int IChannel::SendValuesToString(unsigned int _InLen, unsigned int _InType, std::string& _InStr)
{


	auto buffer = this->mICMsg->buffer;
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
		int iRet = bufferevent_write(buffer, _SendStr.c_str(), _SendStr.size());
		if (iRet < 0)
		{
			return iRet;
		}

	}
	else
		return 0;
}



int IChannel::ICannel_Init(IChannelMsg* Info)
{
	if (Info == NULL)
	{
		SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "Info is NULL ");
		return -1;
	}
	this->mICMsg = Info;

	this->StrByte = new StringByte;
	return 0;
}

void IChannel::IChannel_free()
{

	if (this->StrByte != NULL)
	{
		delete this->StrByte;
		this->StrByte == nullptr;
	}
	if (this->mICMsg != NULL)
	{
		delete this->mICMsg;
		this->mICMsg = nullptr;
	}
}

int IChannel::DatatoInStrByte(char* buf)
{
	if (buf == NULL)
	{
		SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "buf is null ");
		return -1;
	}
	this->StrByte->_InStr = buf;
	return 0;
}