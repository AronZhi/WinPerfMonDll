#include "pch.h"
#include "WinPerfMonDataHandler.h"
#include "Help.h"

WinPerfMonDataHandler::WinPerfMonDataHandler()
	:_proc_line(0), _sys_line(0), _net_line(0)
{
	_proc_csv.clear();
	_proc_cache.clear();
	_sys_csv.clear();
	_sys_cache.clear();
	_net_csv.clear();
	_net_csv.clear();
}

WinPerfMonDataHandler::~WinPerfMonDataHandler()
{
	if (_proc_csv.is_open())
	{
		if (_proc_cache.tellp() > 0)
			_proc_csv << _proc_cache.str();
		_proc_csv.close();
	}
	if (_sys_csv.is_open())
	{
		if (_sys_cache.tellp() > 0)
			_sys_csv << _sys_cache.str();
		_sys_csv.close();
	}
	if (_net_csv.is_open())
	{
		if (_net_cache.tellp() > 0)
			_net_csv << _net_cache.str();
		_net_csv.close();
	}		
}

void WinPerfMonDataHandler::OpenCsvFile(const std::string& name)
{
	std::string file_name(name);
	if (name == "")
		file_name = std::to_string(LocalTimeStampNow());
	_proc_csv.open((file_name + "_proc.csv").c_str(), std::ios::out | std::ios::app);
	_proc_csv << "Time_Stamp," << "CPU_Time," << "Thread_Count," << "Kernal_Handle_Count," << "Virtual_Mem" << "\n";
	_sys_csv.open((file_name + "_sys.csv").c_str(), std::ios::out | std::ios::app);
	_sys_csv << "Time_Stamp," << "CPU_Time," << "CPU_Perf," << "CPU_Freq," << "Mem" << "\n";
	_net_csv.open((file_name + "_net.csv").c_str(), std::ios::out | std::ios::app);
	_net_csv << "Time_Stamp," << "Received," << "Sent" << "\n";
}

void WinPerfMonDataHandler::WriteSysPrefLine(const std::array<double, Sys_Total>& datas, time_t time_stamp)
{
	_sys_cache << time_stamp << ",";
	for (int i = 0; i < Sys_Total; i++)
	{
		datas[i] > 0.0 ? _sys_cache << datas[i] : _sys_cache << "";
		i == (Sys_Total - 1) ? _sys_cache << "\n" : _sys_cache << ",";
	}
	/*
	if (0 == _sys_csv.tellp() % 1024)
		_sys_csv.flush();
	*/
	_sys_line += 1;
	if (0 == _sys_line % 100)
	{
		_sys_csv << _sys_cache.str();
		_sys_csv.flush();
		_sys_cache.clear();
	}
}

void WinPerfMonDataHandler::WriteNetPrefLine(const std::array<double, Net_Total>& datas, time_t time_stamp)
{
	_net_cache << time_stamp << ",";
	for (int i = 0; i < Net_Total; i++)
	{
		datas[i] > 0.0 ? _net_cache << datas[i] : _net_cache << "";
		i == (Net_Total - 1) ? _net_cache << "\n" : _net_cache << ",";
	}
	_net_line += 1;
	if (0 == _net_line % 100)
	{
		_net_csv << _net_cache.str();
		_net_csv.flush();
		_net_cache.clear();
	}
}

void WinPerfMonDataHandler::WriteProcPrefLine(const std::array<double, Proc_Total>& datas, time_t time_stamp)
{
	_proc_cache << time_stamp << ",";
	for (int i = 0; i < Proc_Total; i++)
	{
		datas[i] > 0.0 ? _proc_cache << datas[i] : _proc_cache << "";
		i == (Proc_Total - 1) ? _proc_cache << "\n" : _proc_cache << ",";
	}
	_proc_line += 1;
	if (0 == _proc_line % 100)
	{
		_proc_csv << _proc_cache.str();
		_proc_csv.flush();
		_proc_cache.clear();
	}
}