#include "WinPerfMon.h"
#include "pch.h"
#include "WinPerfMon.h"

#include <iostream>
#include <algorithm>

#include "Help.h"
#include "SysSysSnapshot.h"

WinPerfMon::WinPerfMon(int internal) :
	_sample_freq(internal), _query_handle(INVALID_HANDLE_VALUE)
{
	_sysCounters.fill(INVALID_HANDLE_VALUE);
	_netCounters.fill(INVALID_HANDLE_VALUE);
	_procCpuCounters.fill(INVALID_HANDLE_VALUE);
	_procKernalHandlerCounters.fill(INVALID_HANDLE_VALUE);
	_procThreadCounters.fill(INVALID_HANDLE_VALUE);
	_procVirtualMemoryCounters.fill(INVALID_HANDLE_VALUE);
	if (ERROR_SUCCESS != PdhOpenQuery(NULL, NULL, &_query_handle))
		_query_handle = INVALID_HANDLE_VALUE;
}

WinPerfMon::~WinPerfMon()
{
	if (_query_handle != INVALID_HANDLE_VALUE)
		CloseQuery();
}

void WinPerfMon::AddCounter(const std::string& cmd, HCOUNTER* p_counter)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
	{
		std::cout << "start query fail" << std::endl;
		return;
	}
	std::wstring wcmd(L"");
	SToWs(cmd, wcmd);
	PDH_STATUS res = PdhAddCounter(_query_handle, wcmd.c_str(), NULL, p_counter);
	if (ERROR_SUCCESS != res)
	{
		*p_counter = INVALID_HANDLE_VALUE;
		std::cout << "Add counter fail, " << res << " " << cmd << std::endl;
	}
}

void WinPerfMon::AddSysCounter()
{
	AddCounter("\\Processor Information(_Total)\\% Processor Time", &_sysCounters[Sys_CPU_Time]);
	AddCounter("\\Processor Information(0,_Total)\\% Processor Performance", &_sysCounters[Sys_CPU_Perf]);
	AddCounter("\\Processor Information(0,_Total)\\Processor Frequency", &_sysCounters[Sys_CPU_Freq]);
	AddCounter("\\Memory\\Available MBytes", &_sysCounters[Sys_Mem]);
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
	double ret_value = ERROR_PERF_VALE;
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

double WinPerfMon::GetCountersDoubleValue(std::array<HCOUNTER, MAX_PROC_COUNT>& arr)
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

void WinPerfMon::AddNetCounter(const std::string& net_adapter_name)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
		return;
	std::string adapter(net_adapter_name);
	if (adapter == "")
	{
		SysSnapshot snapshot;
		snapshot.GetNetAdapterName(adapter);
	}
	AddCounter("\\Network Interface(" + adapter + ")\\Bytes Received/sec", &_netCounters[Net_Received]);
	AddCounter("\\Network Interface(" + adapter + ")\\Bytes Sent/sec", &_netCounters[Net_Sent]);
}

void WinPerfMon::InitProcessCounter(std::vector<std::string>& proc_lst)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
		return;
	if (proc_lst.size() >= MAX_PROC_COUNT)
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

void WinPerfMon::AddMonitProcessCounter(const std::string& process_exe)
{

	std::vector<std::string> proc_lst;
	SysSnapshot snapshot;
	snapshot.GetProcAndChildProcName(process_exe, proc_lst);
	InitProcessCounter(proc_lst);
}

void WinPerfMon::AddMonitProcessCounter(DWORD pid)
{
	std::vector<std::string> proc_lst;
	SysSnapshot snapshot;
	snapshot.GetProcAndChildProcName(pid, proc_lst);
	InitProcessCounter(proc_lst);
}

void WinPerfMon::ReadSysInfo()
{
	GetSystemInfo(&_sys_info);
}

double WinPerfMon::GetSysPref(SysPerfField field)
{
	if ((field >= Sys_CPU_Time) && (field < Sys_Total))
		return GetCounterDoubleValue(_sysCounters[field]);
	return ERROR_PERF_VALE;
}

double WinPerfMon::GetNetPref(NetPerfField field)
{
	if ((field >= Net_Received) && (field < Net_Total))
		return GetCounterDoubleValue(_netCounters[field]);
	return ERROR_PERF_VALE;
}

double WinPerfMon::GetProcPref(ProcPerfField field)
{
	double ret = ERROR_PERF_VALE;
	switch (field)
	{
	case Proc_CPU_Time:
		ret = GetCountersDoubleValue(_procCpuCounters);
		break;
	case Proc_Thread_Count:
		ret = GetCountersDoubleValue(_procThreadCounters);
		break;
	case Proc_Kernal_Handle_Count:
		ret = GetCountersDoubleValue(_procKernalHandlerCounters);
		break;
	case Proc_Virtual_Mem:
		{
			ret = GetCountersDoubleValue(_procVirtualMemoryCounters);
			if (ret > 0.0)
				ret = ret / M_;
		}
		break;
	case Proc_Total:
		break;
	default:
		break;
	}
	return ret;
}

DWORD WinPerfMon::GetCPUCount()
{
	return _sys_info.dwNumberOfProcessors;
}