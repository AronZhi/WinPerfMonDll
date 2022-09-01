#include "pch.h"
#include "WinPerfCounter.h"

#include <iostream>
#include <algorithm>

#include "Help.h"
#include "SysInfo.h"

WinPerfCounter::WinPerfCounter(int interval) :
	_sample_time(0), _sample_interval(interval), _query_handle(INVALID_HANDLE_VALUE), _procCpuCounters(nullptr),
	_procKernalHandlerCounters(nullptr), _procThreadCounters(nullptr), _procVirtualMemoryCounters(nullptr)
{
	_proc_lst.clear();
	_sysCounters.fill(INVALID_HANDLE_VALUE);
	_netCounters.fill(INVALID_HANDLE_VALUE);
	if (ERROR_SUCCESS != PdhOpenQuery(NULL, NULL, &_query_handle))
		_query_handle = INVALID_HANDLE_VALUE;
}

WinPerfCounter::~WinPerfCounter()
{
	if (_query_handle != INVALID_HANDLE_VALUE)
		_CloseQuery();
}

void WinPerfCounter::_CloseQuery()
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

void WinPerfCounter::_AddCounter(const std::string& cmd, HCOUNTER* p_counter)
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

bool WinPerfCounter::_AddProcessCounter(std::vector<std::string>& proc_lst)
{
	if (_procCpuCounters || _procThreadCounters || _procKernalHandlerCounters || _procVirtualMemoryCounters)
		return false;
	else
	{
		int count = proc_lst.size();
		_procCpuCounters = new HCOUNTER[count];
		_procKernalHandlerCounters = new HCOUNTER[count];
		_procThreadCounters = new HCOUNTER[count];
		_procVirtualMemoryCounters = new HCOUNTER[count];
		for (int i = 0; i < count; i++)
		{
			_AddCounter("\\Process(" + proc_lst[i] + ")\\% Processor Time", &_procCpuCounters[i]);
			_AddCounter("\\Process(" + proc_lst[i] + ")\\Handle Count", &_procKernalHandlerCounters[i]);
			_AddCounter("\\Process(" + proc_lst[i] + ")\\Thread Count", &_procThreadCounters[i]);
			_AddCounter("\\Process(" + proc_lst[i] + ")\\Virtual Bytes", &_procVirtualMemoryCounters[i]);
		}
		return true;
	}
}

double WinPerfCounter::_GetCounterDoubleValue(HCOUNTER counter)
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
	return ret_value;
}

void WinPerfCounter::AddSysCounter()
{
	_AddCounter("\\Processor Information(_Total)\\% Processor Time", &_sysCounters[Sys_CPU_Time]);
	_AddCounter("\\Processor Information(0,_Total)\\% Processor Performance", &_sysCounters[Sys_CPU_Perf]);
	_AddCounter("\\Processor Information(0,_Total)\\Processor Frequency", &_sysCounters[Sys_CPU_Freq]);
	_AddCounter("\\Memory\\Available MBytes", &_sysCounters[Sys_Mem]);
}

void WinPerfCounter::AddNetCounter(const std::string& net_adapter_name)
{
	if (INVALID_HANDLE_VALUE == _query_handle)
		return;
	std::string adapter(net_adapter_name);
	std::replace(adapter.begin(), adapter.end(), '(', '[');
	std::replace(adapter.begin(), adapter.end(), ')', ']');
	_AddCounter("\\Network Interface(" + adapter + ")\\Bytes Received/sec", &_netCounters[Net_Received]);
	_AddCounter("\\Network Interface(" + adapter + ")\\Bytes Sent/sec", &_netCounters[Net_Sent]);
}

void WinPerfCounter::AddProcessCounter(DWORD pid)
{
	if (_proc_lst.size() > 0)
		return;
	SysInfo snapshot;
	snapshot.GetProcAndChildProcName(pid, _proc_lst);
	_AddProcessCounter(_proc_lst);
}

void WinPerfCounter::AddProcessCounter(const std::string& process_exe)
{
	if (_proc_lst.size() > 0)
		return;
	SysInfo snapshot;
	snapshot.GetProcAndChildProcName(process_exe, _proc_lst);
	_AddProcessCounter(_proc_lst);
}

void WinPerfCounter::BlockCollectData()
{
	PdhCollectQueryData(_query_handle);
	Sleep(_sample_interval * 1000);
	PdhCollectQueryData(_query_handle);
	_sample_time += _sample_interval;
}

void WinPerfCounter::GetSysPref(SysPerfData& ret)
{
	if (_sysCounters[Sys_CPU_Time] == INVALID_HANDLE_VALUE)
		return;
	ret.time = _sample_time;
	ret.sys_cpu_time = _GetCounterDoubleValue(_sysCounters[Sys_CPU_Time]);
	ret.sys_cpu_perf = _GetCounterDoubleValue(_sysCounters[Sys_CPU_Perf]);
	ret.sys_cpu_freq = _GetCounterDoubleValue(_sysCounters[Sys_CPU_Freq]);
	ret.sys_mem = _GetCounterDoubleValue(_sysCounters[Sys_Mem]);
}

void WinPerfCounter::GetNetPref(NetPerfData& ret)
{
	if (_netCounters[Net_Received] == INVALID_HANDLE_VALUE)
		return;
	ret.time = _sample_time;
	ret.net_received = _GetCounterDoubleValue(_netCounters[Net_Received]);
	ret.net_sent = _GetCounterDoubleValue(_netCounters[Net_Sent]);
}

void WinPerfCounter::GetProcPref(std::vector<ProcPerfData>& ret)
{
	if (0 == _proc_lst.size())
		return;
	ret.clear();
	ret.resize(_proc_lst.size());
	for (int i = 0; i < _proc_lst.size(); i++)
	{
		ProcPerfData& data = ret[i];
		data.time = _sample_time;
		data.proc_name = _proc_lst[i];
		data.proc_cpu_time = _GetCounterDoubleValue(_procCpuCounters[i]);
		data.proc_thread_count = _GetCounterDoubleValue(_procThreadCounters[i]);
		data.proc_kernal_handle_Count = _GetCounterDoubleValue(_procKernalHandlerCounters[i]);
		data.proc_virtual_mem = _GetCounterDoubleValue(_procVirtualMemoryCounters[i]);
	}
}