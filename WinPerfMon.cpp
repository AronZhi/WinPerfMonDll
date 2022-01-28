#include "WinPerfMon.h"
#include "pch.h"
#include "WinPerfMon.h"

#include <iostream>
#include <algorithm>

#include "Help.h"
#include "SysSysSnapshot.h"

WinPerfMon::WinPerfMon(int internal) :
	_sample_freq(internal), _query_handle(INVALID_HANDLE_VALUE), _cpuCounter(INVALID_HANDLE_VALUE), _memCounter(INVALID_HANDLE_VALUE),
	_netReceive(INVALID_HANDLE_VALUE), _netSend(INVALID_HANDLE_VALUE)
{
	_procCpuCounters.fill(INVALID_HANDLE_VALUE);
	_procKernalHandlerCounters.fill(INVALID_HANDLE_VALUE);
	_procThreadCounters.fill(INVALID_HANDLE_VALUE);
	_procVirtualMemoryCounters.fill(INVALID_HANDLE_VALUE);
	InitSysCounter();
}

WinPerfMon::~WinPerfMon()
{
	if (_query_handle != INVALID_HANDLE_VALUE)
		CloseQuery();
}

void WinPerfMon::AddCounter(const std::string& cmd, HCOUNTER* p_counter)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
		return;
	std::wstring wcmd(L"");
	SToWs(cmd, wcmd);
	PDH_STATUS res = PdhAddCounter(_query_handle, wcmd.c_str(), NULL, p_counter);
	if (ERROR_SUCCESS != res)
	{
		*p_counter = INVALID_HANDLE_VALUE;
		std::cout << "Add counter fail, " << res << " " << cmd << std::endl;
	}
}

void WinPerfMon::InitSysCounter()
{
	PDH_STATUS res = PdhOpenQuery(NULL, NULL, &_query_handle);
	if (ERROR_SUCCESS != res)
	{
		std::cout << "Create Query fail, " << res << std::endl;
		return;
	}
	else
	{
		AddCounter("\\Processor Information(_Total)\\% Processor Time", &_cpuCounter);
		AddCounter("\\Memory\\Available MBytes", &_memCounter);
	}
}

void WinPerfMon::CloseQuery()
{
	if (_query_handle != INVALID_HANDLE_VALUE)
	{
		PDH_STATUS res = PdhCloseQuery(_query_handle);
		if (ERROR_SUCCESS == res)
			_query_handle = INVALID_HANDLE_VALUE;
		else
			std::cout << "close query fail, res: " << res << std::endl;
	}
}

double WinPerfMon::GetCounterDoubleValue(HCOUNTER counter)
{
	double ret_value = -100.0;
	if (INVALID_HANDLE_VALUE == counter)
		return ret_value;
	PDH_FMT_COUNTERVALUE pdhValue;
	DWORD dwValue = 0.0;
	PDH_STATUS res = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
	if (ERROR_SUCCESS == res)
	{
		ret_value = pdhValue.doubleValue;
	}
	else
	{
		std::cout << "Get Value Error " << res << std::endl;
	}
	return ret_value;
}

double WinPerfMon::GetCountersDoubleValue(std::array<HCOUNTER, 100>& arr)
{
	double value = 0.0;
	double tmp = 0.0;
	for (auto counter : arr)
	{
		tmp = GetCounterDoubleValue(counter);
		tmp > 0.0 ? value += tmp : value;
	}
	return value;
}

void WinPerfMon::BlockCollectData()
{
	PdhCollectQueryData(_query_handle);
	Sleep(_sample_freq * 1000);
	PdhCollectQueryData(_query_handle);
}

double WinPerfMon::GetSysCPU()
{
	return GetCounterDoubleValue(_cpuCounter);
}

double WinPerfMon::GetTotalMem()
{
	return GetCounterDoubleValue(_memCounter);
}

double WinPerfMon::GetNetReceive()
{
	return GetCounterDoubleValue(_netReceive) / 1024;
}

double WinPerfMon::GetNetSend()
{
	return GetCounterDoubleValue(_netSend) / 1024;
}

void WinPerfMon::SetTargetNetAdapter(const std::string& name)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
		return;
	std::string adapter(name);
	if (name == "")
	{
		SysSnapshot snapshot;
		snapshot.GetNetAdapterName(adapter);
	}
	AddCounter("\\Network Interface(" + adapter + ")\\Bytes Received/sec", &_netReceive);
	AddCounter("\\Network Interface(" + adapter + ")\\Bytes Sent/sec", &_netSend);
}

void WinPerfMon::InitProcessCounter(std::vector<std::string>& proc_lst)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
		return;
	if (proc_lst.size() > 99)
	{
		std::cout << "to much child process" << std::endl;
		return;
	}
	int index = 0;
	for (auto name : proc_lst)
	{
		AddCounter("\\Process(" + name + ")\\% Processor Time", &_procCpuCounters[index]);
		AddCounter("\\Process(" + name + ")\\Handle Count", &_procKernalHandlerCounters[index]);
		AddCounter("\\Process(" + name + ")\\Thread Count", &_procThreadCounters[index]);
		AddCounter("\\Process(" + name + ")\\Virtual Bytes", &_procVirtualMemoryCounters[index++]);
	}
}

void WinPerfMon::SetMonitProcess(const std::string& process_exe)
{

	std::vector<std::string> proc_lst;
	SysSnapshot snapshot;
	snapshot.GetProcAndChildProcName(process_exe, proc_lst);
	InitProcessCounter(proc_lst);
}

void WinPerfMon::SetMonitProcess(DWORD pid)
{
	std::vector<std::string> proc_lst;
	SysSnapshot snapshot;
	snapshot.GetProcAndChildProcName(pid, proc_lst);
	InitProcessCounter(proc_lst);
}

double WinPerfMon::GetProcCPU()
{
	return GetCountersDoubleValue(_procCpuCounters);
}

double WinPerfMon::GetProcKernalHandleCount()
{
	return GetCountersDoubleValue(_procKernalHandlerCounters);
}

double WinPerfMon::GetProcThreadCount()
{
	return GetCountersDoubleValue(_procThreadCounters);
}

double WinPerfMon::GetProcVirtualMemory()
{
	return GetCountersDoubleValue(_procVirtualMemoryCounters) / 1024 / 1024;
}

void WinPerfMon::ReadSysInfo()
{
	GetSystemInfo(&_sys_info);
}

DWORD WinPerfMon::GetCPUCount()
{
	return _sys_info.dwNumberOfProcessors;
}