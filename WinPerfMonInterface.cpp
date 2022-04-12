#include "pch.h"
#include "WinPerfMonInterface.h"
#include "WinPerfMonitor.h"

bool  Start(const char* json_param)
{
	return WinPerfMonitor::GetInstance().Start(json_param);
}

int Stop()
{
	return WinPerfMonitor::GetInstance().Stop();
}