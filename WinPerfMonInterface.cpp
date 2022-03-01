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

void SetTaskName(const char* name)
{
	if (NULL == name)
		return;
	Controller::GetInstance().SetTaskName(name);
}

void SetSampleFrequence(int second)
{
	if (second < 2)
		return
	Controller::GetInstance().SetSampleFrequence(second);
}

bool  Start()
{
	return Controller::GetInstance().StartLoop();
}

int Stop()
{
	return Controller::GetInstance().StopLoop();
}