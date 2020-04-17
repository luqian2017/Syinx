//Syinx Kernel ,DO NOT EDIT!
/*

				######
			   #########
			  ############
			  #############
			 ##  ###########
			###  ###### #####
			### #######   ####
		   ###  ########## ####
		  ####  ########### ####
		 ####   ###########  #####
		#####   ### ########   #####
	   #####   ###   ########   ######
	  ######   ###  ###########   ######
	 ######   #### ##############  ######
	#######  #####################  ######
	#######  ######################  ######
   #######  ###### #################  ######
   #######  ###### ###### #########   ######
   #######    ##  ######   ######     ######
   #######        ######    #####     #####
	######        #####     #####     ####
	 #####        ####      #####     ###
	  #####       ###        ###      #
		###       ###        ###
		 ##       ###        ###
__________#_______####_______####______________
				我们的未来没有BUG

*/
#pragma  once
#include "GameObject.h"
#include <list>
struct event_base;
struct evconnlistener;
struct bufferevent;
struct evhttp;
struct evkeyval;
struct evhttp_bound_socket;

class IChannel;
class SyinxAdapterResource;
class SyinxPthreadPool;
class SyinxKernel;
class SyinxBase;
class SConnect;
//设置线程安全以及关闭底层套接字  
//set thread and close socket
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING				(LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE)

//设置线程安全以及关闭底层套接字以及设置close标志位  
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING_OR_FREE		(LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE |  LEV_OPT_CLOSE_ON_FREE)

//设置读写循环
#define SET_SOCKETS_EVENT_RDWR								(EV_READ | EV_WRITE | EV_PERSIST)

//设置读循环
//set read and persist
#define SET_SOCKETS_EVENT_RD								(EV_READ | EV_PERSIST)


/*
* 可编辑宏
* 根据不同的设置编辑宏所设置定的值
*/
//设置连接同步数
#define LISTEN_OPT											10

//缓存buffer设置大小
#define READBUFFER											4086

//设置读低水位
#define LIBEVENT_READ_WATERMARK								0

//设置帧同步所需的帧数一秒15帧
#define GAME_SERVER_FRAME_NUMS								15

#define SERVER_MODE_TCP			0x01
#define SERVER_MODE_HTTP		0x02


#define HASJSONKEYS(keys)				if (!doc.HasMember(keys)){										\
																LOG(ERROR)<<"NOT FIND Keys : "<<keys;	\
																return false;							\
															}

#define GETICHANNEL						 (IChannel*)ctx


// Tcp Call Back Function

void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx);

void SyinxKernel_Send_Cb(struct bufferevent* bev, void* ctx);

void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx);

void SIG_HANDLE(int Sig);



class SyinxKernel :public SObject
{

	enum SYINX_LINK_STATUS
	{
		SYINX_LINK_CLOSE ,
		SYINX_LINK_WORK ,
		SYINX_LINK_WAIT,
		SYINX_LINK_MAX,
	};
	
private:
	void					(SyinxKernel::* m_SyinxStatusFunc[SYINX_LINK_MAX])();
	//status function
	void		OnStatusDoAction();
	void		OnStatusDoClose();
	/* Config.ini */
	short					m_Port;				
	int						m_ClientContentNum;								//最大客户端;连接数
	int						m_PthPoolNum;
	int						m_TaskNum;
	int						m_TimerSec;										// timer sec
	bool					mUsePthreadPool;								// 是否启用一个单例的多线程池
	int						m_SvModle;										// 服务器模式


	// libevent Tcp 模块
	event_base*				mSyinxBase;								
	evconnlistener*			mSyinxListen;								
	SyinxAdapterResource*	mSyResource;									//bind AdapterResource
	list<SConnect*>			m_AllConnect;		

	// libevent Http 模块
	evhttp*					mEvHttp;
	evhttp_bound_socket*	mBoundSket;			
public:
	SyinxKernel();
	~SyinxKernel();
	static SyinxKernel& StaticClass();
	bool Config(int argc, char* argv[]);
	bool Initialize();
	void Run();
	void Close();

	SyinxAdapterResource* GetResource();
	event_base*			GetEventBase();
	evconnlistener* GetListener();
	int				GetPthreadPoolNum()const;
	int				GetPthreadTaskNum()const;

	void OnStatusConnect();

	static void ListenerConnect(SConnect* sconnect);
	static void RemoveConnect(SConnect* sconnect);
	/*
	*查看电脑大小端如果是小端则返回1，如果是大端则返回0
	*/
	int	 GetEndian();

	
	int m_nWorkStatus;		// 状态机工作属性
	int m_endian;			// 大小端模式
private:
	int			JudgeSystem(void);					

	/*
	* 初始化适配器
	*/
	bool		SyinxKernelInitAdapter();

	/*
	* 注册信号
	*/
	bool		RegisterSignal();

	/*
	* 服务器模式配置
	*/
	bool		TCPServer();
	bool		HTTPServer();

	void TCPRun();

	void HTTPRun();
};

extern SyinxKernel&				 g_pSyinx;
extern uint64_t					 g_nGameServerSecond;


uint64_t			GetMselTime();
void				Skipping(const int mesltime);


