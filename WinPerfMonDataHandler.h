#pragma once


#include <string>

#include "PerfMonDataDef.h"
#include "nlohmann/json.hpp"

class WinPerfMonDataHandler
{
private:
	std::string _buf;
	nlohmann::json _json_obj;
	int _cpu_count;
	std::string _machine_name;

protected:
	void _SendData(short type);
public:
	WinPerfMonDataHandler(const std::string& machine, int cpu_count = 1) : _buf(""), _cpu_count(1), _machine_name(machine) {}

	void HandleSysData(const SysPerfData& data);
	void HandleNetData(const NetPerfData& data);
	void HandleProcData(const std::vector<ProcPerfData>& data);
};

