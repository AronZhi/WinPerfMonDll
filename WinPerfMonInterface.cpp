#include "pch.h"
#include "WinPerfMonInterface.h"
#include "Controller.h"

void  SetProcName(const char* name)
{
	if (NULL == name)
		return;
	Controller::GetInstance().SetProcName(name);
}

void  SetProcPid(int Pid)
{
	Controller::GetInstance().SetProcPid(Pid);
}

void  SetNetAdapter(const char* name)
{
	if (NULL == name)
		return;
	Controller::GetInstance().SetNetAdapter(name);
}

bool  Start()
{
	return Controller::GetInstance().StartLoop();
}

void Stop()
{
	return Controller::GetInstance().StopLoop();
}