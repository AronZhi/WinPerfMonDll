#pragma once

#include "framework.h"
#include <array>
#include <vector>
#include <string>

#include "PerfMonDataDef.h"

class WinPerfMon
{
private:
	int _sample_freq;
	SYSTEM_INFO _sys_info;
	HQUERY _query_handle;
	std::array<HCOUNTER, Sys_Total> _sysCounters;
	std::array<HCOUNTER, Net_Total> _netCounters;
	std::array<HCOUNTER, MAX_PROC_COUNT> _procCpuCounters;
	std::array<HCOUNTER, MAX_PROC_COUNT> _procKernalHandlerCounters;
	std::array<HCOUNTER, MAX_PROC_COUNT> _procThreadCounters;
	std::array<HCOUNTER, MAX_PROC_COUNT> _procVirtualMemoryCounters;

protected:
	void CloseQuery();
	double GetCounterDoubleValue(HCOUNTER counter);
	double GetCountersDoubleValue(std::array<HCOUNTER, MAX_PROC_COUNT>& arr);
	void InitProcessCounter(std::vector<std::string>& proc_lst);
	void AddCounter(const std::string& cmd, HCOUNTER* p_counter);

public:
	WinPerfMon(int internal = 1);
	virtual ~WinPerfMon();
	void BlockCollectData();
	void ReadSysInfo();

	double GetSysPref(SysPerfField field);
	double GetNetPref(NetPerfField field);
	double GetProcPref(ProcPerfField field);
	DWORD GetCPUCount();

	void AddSysCounter();
	void AddNetCounter(const std::string& net_adapter_name = "");
	void AddMonitProcessCounter(const std::string& process_exe); // most add 100 process cpu counter
	void AddMonitProcessCounter(DWORD pid); // most add 100 process cpu counter
	
};

