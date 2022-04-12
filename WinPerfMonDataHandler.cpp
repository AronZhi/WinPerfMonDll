#include "pch.h"
#include <iostream>

#include "WinPerfMonDataHandler.h"
#include "Help.h"

WinPerfMonDataHandler::~WinPerfMonDataHandler()
{
	if (_proc_file.is_open())
		_proc_file.close();
	if (_sys_file.is_open())
		_sys_file.close();
	if (_net_file.is_open())
		_net_file.close();
}

void WinPerfMonDataHandler::SetLocalFileName(const std::string& name)
{
	_is_save_in_local = true;
	_file_name = name == "" ? std::to_string(LocalTimeStampNow()) : name;
}

void WinPerfMonDataHandler::HandleSysData(const SysPerfData& data)
{
	if (_is_save_in_local)
	{
		if (!_sys_file.is_open())
		{
			_sys_file.open((_file_name + "_sys.csv").c_str(), std::ios::out | std::ios::app);
			_sys_file << "Time," << "CPU_Time," << "CPU_Perf," << "CPU_Freq," << "Mem" << "\n";
		}
		_sys_file << data.time << "," << data.sys_cpu_time << "," << data.sys_cpu_perf << ","
			<< data.sys_cpu_freq << "," << data.sys_mem << "\n";
	}
}

void WinPerfMonDataHandler::HandleNetData(const NetPerfData& data)
{
	if (_is_save_in_local)
	{
		if (!_net_file.is_open())
		{
			_net_file.open((_file_name + "_net.csv").c_str(), std::ios::out | std::ios::app);
			_net_file << "Time," << "Received," << "Sent" << "\n";
		}
		_net_file << data.time << "," << data.net_received << "," << data.net_sent << "\n";
	}
}

void WinPerfMonDataHandler::HandleProcData(const std::vector<ProcPerfData>& data)
{
	if (_is_save_in_local)
	{
		if (!_proc_file.is_open())
		{
			_proc_file.open((_file_name + "_proc.csv").c_str(), std::ios::out | std::ios::app);
			_proc_file << "Name," << "Time," << "CPU_Time," << "Thread_Count," << "Kernal_Handle_Count," << "Virtual_Mem" << "\n";
		}
		for (auto& item : data)
		{
			_proc_file << item.proc_name << "," << item.time << "," << item.proc_cpu_time << "," << item.proc_thread_count << ","
				<< item.proc_kernal_handle_Count << "," << item.proc_virtual_mem << "\n";
		}
	}
}