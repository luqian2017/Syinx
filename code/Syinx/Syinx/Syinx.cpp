#include "include/SyInc.h"
#include "include/SyPthreadPool.h"
#include "include/SyResAdapter.h"
#include "include/SyTaskAdapter.h"
#include "include/Syinx.h"
#include "include/SConnect.h"
#include "include/Tools.hpp"

#include "../pkg/inih/cpp/INIReader.h"
#include "../pkg/easylog/easylogging++.h"
INITIALIZE_EASYLOGGINGPP;

static const struct table_entry {
	const char* extension;
	const char* content_type;
} content_type_table[] = {
	{ "txt", "text/plain" },
	{ "c", "text/plain" },
	{ "h", "text/plain" },
	{ "html", "text/html" },
	{ "htm", "text/htm" },
	{ "css", "text/css" },
	{ "gif", "image/gif" },
	{ "jpg", "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "png", "image/png" },
	{ "pdf", "application/pdf" },
	{ "ps", "application/postscript" },
	{ NULL, NULL },
};

SyinxKernel& g_pSyinx = SyinxKernel::StaticClass();

uint64_t	g_nGameServerSecond = 0;

tm	g_tmGameServerTime;
SyinxKernel& SyinxKernel::StaticClass()
{
	static SyinxKernel syinx;
	return syinx;
}

bool SyinxKernel::Config(int argc, char* argv[])
{
	INIReader Config("./Config.ini");
	if (Config.ParseError())
	{
		return false;
	}
	string LogConfig = Config.GetString("ServerConfig", "LogConfigPath", "");
	if (LogConfig == "")
	{
		return false;
	}

	// Load configuration from file
	el::Configurations conf(LogConfig.c_str());
#ifdef WIN32
	conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
#endif
	// Actually reconfigure all loggers instead
	el::Loggers::reconfigureAllLoggers(conf);

	m_Port = (short)Config.GetInteger("ServerConfig", "Port", -1);
	if (m_Port == -1)
	{
		LOG(ERROR) << "Parse Config Error Port";
		return false;
	}
	m_ClientContentNum = Config.GetInteger("ServerConfig", "ConnectNum", -1);
	if (m_ClientContentNum == -1)
	{
		LOG(ERROR) << "Parse Config Error ConnectNum";
		return false;
	}
	mUsePthreadPool = Config.GetBoolean("PthreadSetting", "UsePthreadPool", false);

	m_PthPoolNum = Config.GetInteger("PthreadSetting", "SetThreadNum", -1);
	if (m_PthPoolNum == -1)
	{
		LOG(ERROR) << "Parse Config Error SetThreadNum";
		return false;
	}

	m_TaskNum = Config.GetInteger("PthreadSetting", "SetTaskNum", -1);
	if (m_TaskNum == -1)
	{
		LOG(ERROR) << "Parse Config Error SetTaskNum";
		return false;
	}
	string Temp = Config.GetString("ServerConfig", "SetModel", "Failed");
	if (Temp == "Failed")
	{
		LOG(ERROR) << "Parse Config Error SetModel";
		return false;
	}
	else if (Temp == "TCP" || Temp == "Tcp" || Temp == "tcp")
	{
		m_SvModle = SERVER_MODE_TCP;
	}
	else if (Temp == "HTTP" || Temp == "Http" || Temp == "http")
	{
		m_SvModle = SERVER_MODE_HTTP;
	}
	return true;
}

SyinxKernel::SyinxKernel()
{
	m_Port						= 0;
	m_ClientContentNum			= 0;
	m_PthPoolNum				= 0;
	m_TaskNum					= 0;
	m_TimerSec					= 0;
	m_endian					= 0;
	mSyinxBase					= nullptr;
	mSyinxListen				= nullptr;
	mSyResource					= nullptr;
	mUsePthreadPool				= false;
	m_nWorkStatus				= SYINX_LINK_CLOSE;
	m_AllConnect.clear();
	m_SvModle					= 0;
	mBoundSket					= nullptr;
	memset(m_SyinxStatusFunc, 0, sizeof(m_SyinxStatusFunc));
	m_SyinxStatusFunc[SYINX_LINK_WORK]			= &SyinxKernel::OnStatusDoAction;
	m_SyinxStatusFunc[SYINX_LINK_CLOSE]			= &SyinxKernel::OnStatusDoClose;

}
SyinxKernel::~SyinxKernel()
{
	m_Port						= 0;
	m_ClientContentNum			= 0;
	m_PthPoolNum				= 0;
	m_TaskNum					= 0;
	m_TimerSec					= 0;
	m_endian					= 0;
	mSyinxBase					= nullptr;
	mSyinxListen				= nullptr;
	mSyResource					= nullptr;
	mUsePthreadPool				= false;
	m_nWorkStatus				= SYINX_LINK_CLOSE;
	m_AllConnect.clear();
	m_SvModle					= 0;
	mBoundSket					= nullptr;
	Close();
}

/**********************************************************************************************************/
/*
TCP回调函数
void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx)
void SyinxKernel_Send_Cb(struct bufferevent* bev, void* ctx)
void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx)
*/
/**********************************************************************************************************/
//读出回调
void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx)
{
	auto ICh = GETICHANNEL;
	if (ICh == nullptr)
		return;

	size_t iRet = bufferevent_read_buffer(bev, ICh->m_Inputevbuffer);
	if (iRet < 0)
		LOG(ERROR)<<"Read buffer Failed Errno:"<<iRet;

}
//写事件回调
void SyinxKernel_Send_Cb(struct bufferevent* bev, void* ctx)
{

}

//事件回调
void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx)
{
	auto mIC = GETICHANNEL;
	auto Res = g_pSyinx.GetResource();
	EVUTIL_SOCKET_ERROR();
	if (what & BEV_EVENT_CONNECTED)
	{

	}
	else if (what & BEV_EVENT_ERROR)
	{
		int Error = evutil_socket_geterror(bufferevent_getfd(bev));
		LOG(ERROR) << "Some error Happend erron:[ " << Error << " ] Describe:" << evutil_socket_error_to_string(Error);
		Res->SocketFdDel(mIC);
	}
	else if (what & BEV_EVENT_TIMEOUT)
	{
		mIC->TimeOut();
	}
	else if (what & BEV_EVENT_EOF)	//客户端主动断开连接
	{
		Res->SocketFdDel(mIC);
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

/**********************************************************************************************************/
/*
HTTP回调函数

*/
/**********************************************************************************************************/
void HttpServerHandle(struct evhttp_request* req, void* arg)
{
	const char* cmdtype;
	struct evkeyvalq* headers;
	struct evkeyval* header;
	struct evbuffer* buf;

	switch (evhttp_request_get_command(req)) {
	case EVHTTP_REQ_GET: cmdtype = "GET"; break;
	case EVHTTP_REQ_POST: cmdtype = "POST"; break;
	case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
	case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
	case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
	case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
	case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
	case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
	case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
	default: cmdtype = "unknown"; break;
	}

	printf("Received a %s request for %s\nHeaders:\n",
		cmdtype, evhttp_request_get_uri(req));

	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header;
		header = header->next.tqe_next) {
		printf("  %s: %s\n", header->key, header->value);
	}
	
	buf = evhttp_request_get_input_buffer(req);
	evbuffer_add_printf(buf, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));
	puts("Input data: <<<");
	while (evbuffer_get_length(buf)) {
		int n;
		char cbuf[128];
		n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
		if (n > 0)
			(void)fwrite(cbuf, 1, n, stdout);
	}
	puts(">>>");
	evbuffer_add_printf(buf, "<html>");
	evbuffer_add_printf(buf, "<head><title>MyHttpServer</title></head>");
	evbuffer_add_printf(buf, "<body>");
	evbuffer_add_printf(buf, "</body>");
	evbuffer_add_printf(buf, "</html>");

	evhttp_send_reply(req, HTTP_OK, "OK", NULL);
}

void SIG_HANDLE(int Sig)
{
	switch (Sig)
	{
	case SIGINT:
		g_pSyinx.Close();
		break;
#ifdef __linux__
	case SIGQUIT:
		g_pSyinx.Close();
		break;
#endif
	default:
		break;
	}
}



//如果有客户端连接
void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg)
{
	struct bufferevent* buffer = nullptr;
	buffer = bufferevent_socket_new(g_pSyinx.GetEventBase(), fd, BEV_OPT_CLOSE_ON_FREE);
	if (nullptr == buffer)
	{
		LOG(ERROR) << "bufferevent_socket_new is failed";
		return;
	}
	g_pSyinx.GetResource()->AllocationIChannel(buffer, fd);
}

bool SyinxKernel::Initialize()
{
	m_endian = JudgeSystem();

	if (!SyinxKernelInitAdapter())
	{
		LOG(ERROR) << "SyinxKernel_InitAdapter() Failed";
		return false;
	}
	RegisterSignal();

	if (m_SvModle & SERVER_MODE_TCP)
	{
		LOG(INFO) << "TCP Server is Open";
		return TCPServer();
	}
	else if (m_SvModle & SERVER_MODE_HTTP)
	{
		LOG(INFO) << "HTTP Server is Open";
		return HTTPServer();
	}
	else
	{
		LOG(ERROR) << m_SvModle << "Not Find";
		return false;
	}

}

bool SyinxKernel::SyinxKernelInitAdapter()
{
	//初始化线程管理器
	if (m_PthPoolNum > 1 && mUsePthreadPool)
	{
		if (g_SyinxPthPool.Initialize(m_PthPoolNum, m_TaskNum) == NULL)
		{
			LOG(ERROR) << "Pthread init failed";
		}
	}
	else
		LOG(INFO) << "Pthread is not Init/ set Pool num = 0 or UsePthreadPool = false";
	


	//初始化资源管理器
	mSyResource = CreateObject<SyinxAdapterResource>::Instance(m_ClientContentNum);
	if (nullptr == mSyResource)
	{
		//log
		LOG(ERROR) << "new SyinxAdapterResource failed...";
		return false;
	}
	if (mSyResource->Initialize())
	{
		LOG(INFO) << "SyinxAdapterResource_Init Success";
	}
	else
	{
		LOG(ERROR) << "SyinxAdapterResource_Init failed...";
		return false;
	}
	return true;
}

bool SyinxKernel::RegisterSignal()
{
	signal(SIGINT, SIG_HANDLE);
#ifdef __linux__
	signal(SIGQUIT, SIG_HANDLE);
#endif
	return true;
}

bool SyinxKernel::TCPServer()
{
	//初始化结构体
	struct sockaddr_in _Serveraddr;
	memset(&_Serveraddr, 0, sizeof(_Serveraddr));
	_Serveraddr.sin_family = AF_INET;
	_Serveraddr.sin_port = htons(m_Port);

#ifdef WIN32
	WORD wVersionRequested = 0;
	WSADATA wsaData;
	int err;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		LOG(ERROR) << "WSAStartup failed with error: %d\n" << err;
		return false;
	}
#endif

	//创建句柄
	if (!mSyinxBase)
	{
		struct event_config* cfg = event_config_new();
		if (cfg) {
			/*
			To access base security , unallocable use thread call base
			*/
#if WIN32
			evthread_use_windows_threads();
			event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
#elif defined (__linux__)
			event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK);
#endif
			mSyinxBase = event_base_new_with_config(cfg);
			if (mSyinxBase == nullptr)
			{
				LOG(ERROR) << "event_base_new() Failed";
				return false;
			}
			event_config_free(cfg);
		}
	}

	//bind

	mSyinxListen = evconnlistener_new_bind(mSyinxBase, SyinxKernel_Listen_CB, nullptr,
		SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING, 10, (const sockaddr*)&_Serveraddr, sizeof(_Serveraddr));
	if (nullptr == mSyinxListen)
	{
		LOG(ERROR) << "Create Base  : evconnlistener_new_bind is failed";
		return false;
}

	OnStatusConnect();
	m_nWorkStatus = SYINX_LINK_WORK;
	return true;
}

bool SyinxKernel::HTTPServer()
{
#ifdef WIN32
	WORD wVersionRequested = 0;
	WSADATA wsaData;
	int err;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		LOG(ERROR) << "WSAStartup failed with error: %d\n" << err;
		return false;
	}
	
#endif
	//创建句柄
	if (!mSyinxBase)
	{
		struct event_config* cfg = event_config_new();
		if (cfg) {
			/*
			To access base security , unallocable use thread call base
			*/
#if WIN32
			evthread_use_windows_threads();
			event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
#elif defined (__linux__)
			event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK);
#endif
			mSyinxBase = event_base_new_with_config(cfg);
			if (mSyinxBase == nullptr)
			{
				LOG(ERROR) << "event_base_new() Failed";
				return false;
			}
			event_config_free(cfg);
		}
	}
	mEvHttp = evhttp_new(mSyinxBase);
	if (!mEvHttp)
	{
		LOG(ERROR) << "evhttp_new() Failed";
		return false;
	}
	evhttp_set_gencb(mEvHttp, HttpServerHandle, nullptr);
	evhttp_set_cb(mEvHttp, "/test", HttpServerHandle, nullptr);

	mBoundSket = evhttp_bind_socket_with_handle(mEvHttp, "127.0.0.1", m_Port);
	if (!mBoundSket)
	{
		LOG(ERROR) << "evhttp_bind_socket_with_handle() Failed";
		return false;
	}

	
	m_nWorkStatus = SYINX_LINK_WORK;
	return true;
}

void SyinxKernel::TCPRun()
{
	int iRet = 0;
	uint64_t BeginMeslTime = 0;
	uint64_t NextMeslTime = 0;

	uint64_t BeginPingTime = 0;
	uint64_t NextPingTime = 0;
	while (m_nWorkStatus)
	{
		iRet = event_base_loop(mSyinxBase, EVLOOP_NONBLOCK);
		if (-1 == iRet)
		{
			LOG(ERROR) << "event_base_loop go wrong  ";
			return;
		}
		BeginMeslTime = GetMselTime();
		if (BeginMeslTime < NextMeslTime)
		{
			Skipping(1);
			continue;
		}
		NextMeslTime = BeginMeslTime + 1000 / GAME_SERVER_FRAME_NUMS;

		g_nGameServerSecond = time(nullptr);

		time_t tmGameServerTime = g_nGameServerSecond;

#if defined(WIN32) || defined(WIN64)
		localtime_s(&g_tmGameServerTime, &tmGameServerTime);
#elif defined(__linux)
		localtime_r(&tmGameServerTime, &g_tmGameServerTime);
#endif
		(this->*m_SyinxStatusFunc[m_nWorkStatus])();

	}
}

void SyinxKernel::HTTPRun()
{
	int iRet = 0;
	while (m_nWorkStatus)
	{
		iRet = event_base_loop(mSyinxBase, EVLOOP_ONCE);
		if (-1 == iRet)
		{
			LOG(ERROR) << "event_base_loop go wrong ";
			return;
		}

	}
}


void SyinxKernel::OnStatusDoAction()
{
	mSyResource->GameServerDoAction();
}

void SyinxKernel::OnStatusDoClose()
{
	Close();
}

void SyinxKernel::Run()
{
	if (m_SvModle & SERVER_MODE_TCP)
	{
		TCPRun();
	}
	else if (m_SvModle & SERVER_MODE_HTTP)
	{
		HTTPRun();
	}
	else
	{
		LOG(ERROR) << m_SvModle << "Not Exists";
		return;
	}
}

void SyinxKernel::Close()
{
	if (m_nWorkStatus == SYINX_LINK_CLOSE)
		return;
	m_nWorkStatus = SYINX_LINK_CLOSE;


	if (mSyResource)
	{
		mSyResource->Close();
		delete mSyResource;
		mSyResource = nullptr;
	}
#ifdef WIN32
	WSACleanup();
#endif 
	RELEASE_FROM_FUNCTION(event_base_free, mSyinxBase);

	RELEASE_FROM_FUNCTION(evhttp_free, mEvHttp);

	for (auto Iter : m_AllConnect)
	{
		Iter->Close();
	}
	g_SyinxPthPool.Close();
	puts("Syinx is close! \nPress any key to continue!...");
	getchar();
	exit(0);
}

int SyinxKernel::JudgeSystem(void)
{
	int a = 1;
	//如果是小端则返回1，如果是大端则返回0
	return *(char*)&a;
}

SyinxAdapterResource* SyinxKernel::GetResource()
{
	return mSyResource;
}
event_base* SyinxKernel::GetEventBase()
{
	return mSyinxBase != nullptr ? mSyinxBase : nullptr;
}
evconnlistener* SyinxKernel::GetListener()
{
	return mSyinxListen != nullptr ? mSyinxListen : nullptr;
}




int SyinxKernel::GetPthreadPoolNum() const
{
	return m_PthPoolNum;
}

int SyinxKernel::GetPthreadTaskNum() const
{
	return m_TaskNum;
}

void SyinxKernel::OnStatusConnect()
{
	auto ConServer = SConnect::Connect("192.168.61.128", 10012, "test");
	if (ConServer == nullptr)
	{
		LOG(ERROR) << "Failed";
		return;
	}
}

void SyinxKernel::ListenerConnect(SConnect* sconnect)
{
	g_pSyinx.m_AllConnect.push_back(sconnect);
}

void SyinxKernel::RemoveConnect(SConnect* sconnect)
{
	g_pSyinx.m_AllConnect.remove(sconnect);
}

int SyinxKernel::GetEndian()
{
	return m_endian;
}

uint64_t GetMselTime()
{
#ifdef WIN32
	return  GetTickCount();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

void Skipping(const int mesltime)
{
#ifdef WIN32
	Sleep(mesltime);
#else
	usleep(mesltime*1000);
#endif
}
