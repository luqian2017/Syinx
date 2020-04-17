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
				���ǵ�δ��û��BUG

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
//�����̰߳�ȫ�Լ��رյײ��׽���  
//set thread and close socket
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING				(LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE)

//�����̰߳�ȫ�Լ��رյײ��׽����Լ�����close��־λ  
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING_OR_FREE		(LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE |  LEV_OPT_CLOSE_ON_FREE)

//���ö�дѭ��
#define SET_SOCKETS_EVENT_RDWR								(EV_READ | EV_WRITE | EV_PERSIST)

//���ö�ѭ��
//set read and persist
#define SET_SOCKETS_EVENT_RD								(EV_READ | EV_PERSIST)


/*
* �ɱ༭��
* ���ݲ�ͬ�����ñ༭�������ö���ֵ
*/
//��������ͬ����
#define LISTEN_OPT											10

//����buffer���ô�С
#define READBUFFER											4086

//���ö���ˮλ
#define LIBEVENT_READ_WATERMARK								0

//����֡ͬ�������֡��һ��15֡
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
	int						m_ClientContentNum;								//���ͻ���;������
	int						m_PthPoolNum;
	int						m_TaskNum;
	int						m_TimerSec;										// timer sec
	bool					mUsePthreadPool;								// �Ƿ�����һ�������Ķ��̳߳�
	int						m_SvModle;										// ������ģʽ


	// libevent Tcp ģ��
	event_base*				mSyinxBase;								
	evconnlistener*			mSyinxListen;								
	SyinxAdapterResource*	mSyResource;									//bind AdapterResource
	list<SConnect*>			m_AllConnect;		

	// libevent Http ģ��
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
	*�鿴���Դ�С�������С���򷵻�1������Ǵ���򷵻�0
	*/
	int	 GetEndian();

	
	int m_nWorkStatus;		// ״̬����������
	int m_endian;			// ��С��ģʽ
private:
	int			JudgeSystem(void);					

	/*
	* ��ʼ��������
	*/
	bool		SyinxKernelInitAdapter();

	/*
	* ע���ź�
	*/
	bool		RegisterSignal();

	/*
	* ������ģʽ����
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


