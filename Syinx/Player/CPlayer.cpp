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
	//std::cout << "��ҵ�½" << std::endl;
}

void CPlayer::Logout()
{
	//std::cout << "����˳�" << std::endl;
}

void CPlayer::Close()
{

}

void CPlayer::TimeOut()
{
}
