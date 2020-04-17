#pragma once
#include <iostream>
#include <fstream>
#include <istream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <random>

#include <list>
#include <vector>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace std;

#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/listener.h"
#include "event2/buffer.h"
#include "event2/thread.h"
#include "event2/http.h"
#include "event2/util.h"
#include "event2/keyvalq_struct.h"
#include "pthread.h"

#elif defined (__linux__)
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include <pthread.h>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <mysql/mysql.h>
#endif

#ifdef _DEBUG 
#pragma warning(push)
#pragma warning(disable:4018)
#pragma warning(disable:4819)
#endif





