#pragma once

#include <fstream>
#include <string>
#include <array>

#include "PerfMonDataDef.h"

class WinPerfMonDataHandler
{
private:
	std::ofstream _proc_csv;
	int _proc_line;
	std::ofstream _sys_csv;
	int _sys_line;
	std::ofstream _net_csv;
	int _net_line;

public:
	WinPerfMonDataHandler();
	~WinPerfMonDataHandler();
	
	/* save as csv file */
	void OpenCsvFile(const std::string& name = "");
	void WriteSysPrefLine(const std::array<double, Sys_Total>& datas, time_t time_stamp);
	void WriteNetPrefLine(const std::array<double, Net_Total>& datas, time_t time_stamp);
	void WriteProcPrefLine(const std::array<double, Proc_Total>& datas, time_t time_stamp);
};

