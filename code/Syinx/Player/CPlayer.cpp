#include "../Syinx/include/SyInc.h"
#include "../Syinx/include/Syinx.h"
#include "CPlayer.h"

CPlayer::CPlayer(IChannel* inIch):m_ICh(inIch)
{
	
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Initialize()
{
	return true;
}

void CPlayer::Login()
{
	std::cout << "Time:" << g_nGameServerSecond << " " << m_ICh->GetIChannelUniqueID() << " Login" << std::endl;

}

void CPlayer::Logout()
{
	std::cout << "Time:" << g_nGameServerSecond << " " << m_ICh->GetIChannelUniqueID() << " Logout" << std::endl;
}

void CPlayer::Close()
{

}

void CPlayer::TimeOut()
{
}
