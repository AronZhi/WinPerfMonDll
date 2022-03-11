#include "pch.h"
#include "Controller.h"

#include <iostream>
#include <array>

#include "WinPerfMon.h"
#include "Help.h"
#include "PerfMonDataDef.h"
#include "WinPerfMonDataHandler.h"

Controller::~Controller()
{
	StopLoop();
}

bool Controller::StartLoop()
{
	try
	{
		if (_run)
			return false;
		else
			_run = true;
		_workThread = std::thread(std::mem_fun(&Controller::Work), this);
		return true;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return false;
}

int Controller::StopLoop()
{
	try
	{
		if (_run)
		{
			_run = false;
			_workThread.join();
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "stop thread fail, please restart process" << std::endl;
		return -1;
	}
	return 0;
}

void Controller::Work()
{
	if (this->_sample_freq < 1)
		this->_sample_freq = 1;
	WinPerfMon monitor(this->_sample_freq);
	if (this->_proc_pid > 0)
		monitor.AddMonitProcessCounter(this->_proc_pid);
	else if (this->_proc_name != "")
		monitor.AddMonitProcessCounter(this->_proc_name);
	else
		return;
	monitor.AddSysCounter();
	monitor.AddNetCounter(this->_net_adapter);
	monitor.ReadSysInfo();
	std::array<double, Sys_Total> sys_arry;
	std::array<double, Proc_Total> proc_arry;
	std::array<double, Net_Total> net_arry;
	WinPerfMonDataHandler dataHandler(this->_task_name);
	time_t time = 0;
	int cpu_cout = monitor.GetCPUCount();
	while (_run)
	{
		monitor.BlockCollectData();
		time = LocalTimeStampNow();
		sys_arry[Sys_CPU_Time] = monitor.GetSysPref(Sys_CPU_Time);
		sys_arry[Sys_CPU_Perf] = monitor.GetSysPref(Sys_CPU_Perf);
		sys_arry[Sys_CPU_Freq] = monitor.GetSysPref(Sys_CPU_Freq);
		sys_arry[Sys_Mem] = monitor.GetSysPref(Sys_Mem);
		net_arry[Net_Received] = monitor.GetNetPref(Net_Received);
		net_arry[Net_Sent] = monitor.GetNetPref(Net_Sent);
		proc_arry[Proc_CPU_Time] = monitor.GetProcPref(Proc_CPU_Time);
		proc_arry[Proc_Thread_Count] = monitor.GetProcPref(Proc_Thread_Count);
		proc_arry[Proc_Kernal_Handle_Count] = monitor.GetProcPref(Proc_Kernal_Handle_Count);
		proc_arry[Proc_Virtual_Mem] = monitor.GetProcPref(Proc_Virtual_Mem);
		dataHandler.HandleSysData(sys_arry, time);
		dataHandler.HandleNetData(net_arry, time);
		dataHandler.HandleProcData(proc_arry, time);
	}
}
