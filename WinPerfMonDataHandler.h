#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <array>

#include <array>

#include "PerfMonDataDef.h"
#include "LocalBackup.h"

class WinPerfMonDataHandler
{
private:
	bool _is_send_metric;
	LocalBackup _local_backup;

public:
	WinPerfMonDataHandler(const std::string& local_file_name = "");

	void HandleSysData(const std::array<double, Sys_Total>& datas, time_t time_stamp);
	void HandleNetData(const std::array<double, Net_Total>& datas, time_t time_stamp);
	void HandleProcData(const std::array<double, Proc_Total>& datas, time_t time_stamp);
};

