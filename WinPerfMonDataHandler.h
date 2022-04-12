#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "PerfMonDataDef.h"

class WinPerfMonDataHandler
{
private:
	bool _is_save_in_local;
	std::string _file_name;
	std::ofstream _sys_file;
	std::ofstream _net_file;
	std::ofstream _proc_file;

public:
	WinPerfMonDataHandler() : _is_save_in_local(true) {}
	~WinPerfMonDataHandler();
	void SetLocalFileName(const std::string& name);

	void HandleSysData(const SysPerfData& data);
	void HandleNetData(const NetPerfData& data);
	void HandleProcData(const std::vector<ProcPerfData>& data);
};

