#include "pch.h"
#include "LocalBackup.h"
#include "Help.h"

LocalBackup::LocalBackup(const std::string& name)
	:_proc_line(0), _sys_line(0), _net_line(0)
{
	_proc_file.clear();
	_proc_cache.clear();
	_sys_file.clear();
	_sys_cache.clear();
	_net_file.clear();
	_net_file.clear();

	std::string file_name(name);
	if (name == "")
		file_name = std::to_string(LocalTimeStampNow());
	_proc_file.open((file_name + "_proc.csv").c_str(), std::ios::out | std::ios::app);
	_proc_file << "Time_Stamp," << "CPU_Time," << "Thread_Count," << "Kernal_Handle_Count," << "Virtual_Mem" << "\n";
	_sys_file.open((file_name + "_sys.csv").c_str(), std::ios::out | std::ios::app);
	_sys_file << "Time_Stamp," << "CPU_Time," << "CPU_Perf," << "CPU_Freq," << "Mem" << "\n";
	_net_file.open((file_name + "_net.csv").c_str(), std::ios::out | std::ios::app);
	_net_file << "Time_Stamp," << "Received," << "Sent" << "\n";
}

LocalBackup::~LocalBackup()
{
	if (_proc_file.is_open())
	{
		if (_proc_cache.tellp() > 0)
			_proc_file << _proc_cache.str();
		_proc_file.close();
	}
	if (_sys_file.is_open())
	{
		if (_sys_cache.tellp() > 0)
			_sys_file << _sys_cache.str();
		_sys_file.close();
	}
	if (_net_file.is_open())
	{
		if (_net_cache.tellp() > 0)
			_net_file << _net_cache.str();
		_net_file.close();
	}
}

void LocalBackup::WriteFile(std::ofstream& file, std::stringstream& cache, int& line)
{
	line += 1;
	if (0 == line % 100)
	{
		file << cache.str();
		file.flush();
		cache.clear();
	}
}

void LocalBackup::WriteSysPrefLine(const std::array<double, Sys_Total>& datas, time_t time_stamp)
{
	_sys_cache << time_stamp << ",";
	for (int i = 0; i < Sys_Total; i++)
	{
		datas[i] > 0.0 ? _sys_cache << datas[i] : _sys_cache << "";
		i == (Sys_Total - 1) ? _sys_cache << "\n" : _sys_cache << ",";
	}
	/*
	if (0 == _sys_file.tellp() % 1024)
		_sys_file.flush();
	*/
	WriteFile(_sys_file, _sys_cache, _sys_line);
}

void LocalBackup::WriteNetPrefLine(const std::array<double, Net_Total>& datas, time_t time_stamp)
{
	_net_cache << time_stamp << ",";
	for (int i = 0; i < Net_Total; i++)
	{
		datas[i] > 0.0 ? _net_cache << datas[i] : _net_cache << "";
		i == (Net_Total - 1) ? _net_cache << "\n" : _net_cache << ",";
	}
	WriteFile(_net_file, _net_cache, _net_line);
}

void LocalBackup::WriteProcPrefLine(const std::array<double, Proc_Total>& datas, time_t time_stamp)
{
	_proc_cache << time_stamp << ",";
	for (int i = 0; i < Proc_Total; i++)
	{
		datas[i] > 0.0 ? _proc_cache << datas[i] : _proc_cache << "";
		i == (Proc_Total - 1) ? _proc_cache << "\n" : _proc_cache << ",";
	}
	WriteFile(_proc_file, _proc_cache, _proc_line);
}