#pragma once

#include "framework.h"
#include <array>
#include <vector>
#include <string>

class WinPerfMon
{
private:
	int _sample_freq;
	SYSTEM_INFO _sys_info;
	HQUERY _query_handle;
	HCOUNTER _cpuCounter;
	HCOUNTER _memCounter;
	HCOUNTER _netReceive;
	HCOUNTER _netSend;
	std::array<HCOUNTER, 100> _procCpuCounters;
	std::array<HCOUNTER, 100> _procKernalHandlerCounters;
	std::array<HCOUNTER, 100> _procThreadCounters;
	std::array<HCOUNTER, 100> _procVirtualMemoryCounters;

protected:
	void InitSysCounter();
	void CloseQuery();
	double GetCounterDoubleValue(HCOUNTER counter);
	double GetCountersDoubleValue(std::array<HCOUNTER, 100>& arr);
	void InitProcessCounter(std::vector<std::string>& proc_lst);
	void AddCounter(const std::string& cmd, HCOUNTER* p_counter);

public:
	WinPerfMon(int internal = 1);
	virtual ~WinPerfMon();
	void BlockCollectData();
	double GetSysCPU();  // return cpu usage percentage
	double GetTotalMem(); // reuturn memory MB

	double GetNetReceive(); // kbytes
	double GetNetSend(); // kbytes

	double GetProcCPU();
	double GetProcKernalHandleCount();
	double GetProcThreadCount();
	double GetProcVirtualMemory(); // reuturn memory MB

	DWORD GetCPUCount();

	void SetTargetNetAdapter(const std::string& name = "");
	void SetMonitProcess(const std::string& process_exe); // most add 100 process cpu counter
	void SetMonitProcess(DWORD pid); // most add 100 process cpu counter
	void ReadSysInfo();
};

