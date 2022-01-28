#pragma once

#include "framework.h"
#include <string>
#include <vector>

class SysSnapshot
{
protected:
	bool GetChildProcName(DWORD parent_pid, HANDLE hProcessSnap, std::vector<std::string>& ret_names);
	bool GetProcPid(HANDLE hProcessSnap, const std::string& exe, DWORD& ret_PID);
	void formatProcessName(std::string& exe, int index);
	bool GetProcName(HANDLE hProcessSnap, DWORD PID, std::string& name);
	void SetProceAndChildProcName(std::string& exe, std::vector<std::string>& child_names, std::vector<std::string>& ret);
	void ListProcessModules(DWORD pid);
	void ListProcessThread(DWORD pid);

public:
	bool GetNetAdapterName(std::string& ret);
	bool GetProcAndChildProcName(const std::string& exe, std::vector<std::string>& ret);
	bool GetProcAndChildProcName(DWORD pid, std::vector<std::string>& ret);
};

