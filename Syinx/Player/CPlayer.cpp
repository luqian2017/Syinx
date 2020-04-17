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
	//std::cout << "Íæ¼ÒµÇÂ½" << std::endl;
}

void CPlayer::Logout()
{
	//std::cout << "Íæ¼ÒÍË³ö" << std::endl;
}

void CPlayer::Close()
{

}

void CPlayer::TimeOut()
{
}
