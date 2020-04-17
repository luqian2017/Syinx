#pragma once
class SObject
{
public:

	virtual bool Initialize() = 0;
	virtual void Close() = 0;
	
};

class STask :public SObject
{
public:
	virtual void Login() = 0;
	virtual void Logout() = 0;
	virtual void Clear() = 0;
	virtual void TimeOut() = 0;
	virtual void OnStatusDoAction() = 0;
	virtual void OnStatusDoAcionInPthread() = 0;
	virtual void SetupFrameInputFunc() = 0;
	virtual inline bool GetWhetherBindParseFunc() = 0;
};

//tools

#define RELEASE_FROM_FUNCTION(func,arg)								if(arg!=nullptr){func(arg);arg=nullptr;}

#define RELEASE_FROM_VARIABLE(arg)									if(arg!=nullptr){delete arg;arg=nullptr;}