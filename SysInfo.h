#pragma once

#include "framework.h"
#include <string>
#include <vector>

class SysInfo
{
protected:
	bool _GetChildProcName(DWORD parent_pid, HANDLE hProcessSnap, std::vector<std::string>& ret_names);
	bool _GetProcPid(HANDLE hProcessSnap, const std::string& exe, DWORD& ret_PID);
	bool _GetProcName(HANDLE hProcessSnap, DWORD PID, std::string& name);
	void _SetProceAndChildProcName(std::string& exe, std::vector<std::string>& child_names, std::vector<std::string>& ret);
	void _ListProcessModules(DWORD pid);
	void _ListProcessThread(DWORD pid);

public:
	bool GetNetAdapterName(std::string& ret);
	bool GetProcAndChildProcName(const std::string& exe, std::vector<std::string>& ret);
	bool GetProcAndChildProcName(DWORD pid, std::vector<std::string>& ret);
	int GetCpuCount();
};
