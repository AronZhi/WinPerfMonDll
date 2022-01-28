#include "pch.h"
#include "Controller.h"

#include <iostream>

#include "configor/json.hpp"

#include "WinPerfMon.h"
#include "Help.h"

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

void Controller::StopLoop()
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
	}	
}

void Controller::Work()
{
	if (this->_sample_freq < 1)
		return;
	WinPerfMon monitor(this->_sample_freq);
	if (this->_proc_pid > 0)
		monitor.SetMonitProcess(this->_proc_pid);
	else if (this->_proc_name != "")
		monitor.SetMonitProcess(this->_proc_name);
	else
		return;
	std::cout << this->_proc_pid << " \ " << this->_proc_name << " \ " << this->_net_adapter << std::endl;
	monitor.SetTargetNetAdapter(this->_net_adapter);
	monitor.ReadSysInfo();
	double sys_cpu = 0.0, proc_cpu = 0.0;
	configor::json json_obj;
	while (_run)
	{
		monitor.BlockCollectData();
		json_obj.clear();
		json_obj["time"] = LocalTimeStampNow();
		sys_cpu = monitor.GetSysCPU();
		json_obj["sys_cpu"] = sys_cpu;
		json_obj["sys_available_mem"] = monitor.GetTotalMem();
		json_obj["net_receive"] = monitor.GetNetReceive();
		json_obj["net_send"] = monitor.GetNetSend();
		json_obj["proc_cpu"] = monitor.GetProcCPU();
		json_obj["proc_virtual_mem"] = monitor.GetProcVirtualMemory();
		json_obj["proc_kernal_handle_count"] = monitor.GetProcKernalHandleCount();
		json_obj["proc_thread_count"] = monitor.GetProcThreadCount();
		std::cout << json_obj << std::endl;
	}
}
