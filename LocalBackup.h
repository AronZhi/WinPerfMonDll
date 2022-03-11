#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <array>

#include "PerfMonDataDef.h"

class LocalBackup
{
public:
private:
	std::ofstream _proc_file;
	std::stringstream _proc_cache;
	int _proc_line;
	std::ofstream _sys_file;
	std::stringstream _sys_cache;
	int _sys_line;
	std::ofstream _net_file;
	std::stringstream _net_cache;
	int _net_line;

protected:
	void WriteFile(std::ofstream& file, std::stringstream& cache, int& line);

public:
	LocalBackup(const std::string& name);
	~LocalBackup();

	void WriteSysPrefLine(const std::array<double, Sys_Total>& datas, time_t time_stamp);
	void WriteNetPrefLine(const std::array<double, Net_Total>& datas, time_t time_stamp);
	void WriteProcPrefLine(const std::array<double, Proc_Total>& datas, time_t time_stamp);
};

