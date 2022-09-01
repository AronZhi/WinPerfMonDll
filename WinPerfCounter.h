#pragma once

#include "framework.h"
#include <array>
#include <vector>
#include <string>

#include "PerfMonDataDef.h"

class WinPerfCounter
{
private:
	int64_t _sample_time;
	int _sample_interval;
	HQUERY _query_handle;
	std::array<HCOUNTER, Sys_Total> _sysCounters;
	std::array<HCOUNTER, Net_Total> _netCounters;
	std::vector<std::string> _proc_lst;
	HCOUNTER* _procCpuCounters;
	HCOUNTER* _procKernalHandlerCounters;
	HCOUNTER* _procThreadCounters;
	HCOUNTER* _procVirtualMemoryCounters;

protected:
	void _CloseQuery();
	double _GetCounterDoubleValue(HCOUNTER counter);
	bool _AddProcessCounter(std::vector<std::string>& proc_lst);
	void _AddCounter(const std::string& cmd, HCOUNTER* p_counter);

public:
	WinPerfCounter(int interval = 1);
	virtual ~WinPerfCounter();

	void GetSysPref(SysPerfData& ret);
	void GetNetPref(NetPerfData& ret);
	void GetProcPref(std::vector<ProcPerfData>& ret);

	void AddSysCounter();
	void AddNetCounter(const std::string& net_adapter_name);
	void AddProcessCounter(const std::string& process_exe);
	void AddProcessCounter(DWORD pid);
	void BlockCollectData();

	void SetSampleInterval(int interval) { _sample_interval = interval; }
};