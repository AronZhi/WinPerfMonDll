#pragma once


#include <string>

#include "PerfMonDataDef.h"
#include "nlohmann/json.hpp"

class WinPerfMonDataHandler
{
private:
	std::string _buf;
	nlohmann::json _json_obj;

protected:
	void _SendData();
public:
	void HandleSysData(const SysPerfData& data);
	void HandleNetData(const NetPerfData& data);
	void HandleProcData(const std::vector<ProcPerfData>& data);
};

