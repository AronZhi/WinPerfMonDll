#include "pch.h"
#include "WinPerfMonDataHandler.h"
#include "Help.h"
#include <sstream>

WinPerfMonDataHandler::WinPerfMonDataHandler()
	:_proc_line(0), _sys_line(0), _net_line(0)
{
	_proc_csv.clear();
	_sys_csv.clear();
	_net_csv.clear();
}

WinPerfMonDataHandler::~WinPerfMonDataHandler()
{
	if (_proc_csv.is_open())
		_proc_csv.close();
	if (_sys_csv.is_open())
		_sys_csv.close();
	if (_net_csv.is_open())
		_net_csv.close();
}

void WinPerfMonDataHandler::OpenCsvFile(const std::string& name)
{
	std::string file_name(name);
	if (name == "")
		file_name = std::to_string(LocalTimeStampNow());
	_proc_csv.open((file_name + "_proc.csv").c_str(), std::ios::out | std::ios::app);
	_sys_csv.open((file_name + "_sys.csv").c_str(), std::ios::out | std::ios::app);
	_net_csv.open((file_name + "_net.csv").c_str(), std::ios::out | std::ios::app);
}

void WinPerfMonDataHandler::WriteSysPrefLine(const std::array<double, Sys_Total>& datas, time_t time_stamp)
{
	std::stringstream line;
	line << time_stamp << ",";
	for (auto value : datas)
	{
		line << value << ',';
	}
	line << "\n";
	_sys_csv << line.str();
	/*
	if (0 == _sys_csv.tellp() % 1024)
		_sys_csv.flush();
	*/
	if (0 == _sys_line % 100)
		_sys_csv.flush();
	else
		_sys_line += 1;
}

void WinPerfMonDataHandler::WriteNetPrefLine(const std::array<double, Net_Total>& datas, time_t time_stamp)
{
	std::stringstream line;
	line << time_stamp << ",";
	for (auto value : datas)
	{
		line << value << ',';
	}
	line << "\n";
	_net_csv << line.str();
	if (0 == _net_line % 100)
		_net_csv.flush();
	else
		_net_line += 1;
}

void WinPerfMonDataHandler::WriteProcPrefLine(const std::array<double, Proc_Total>& datas, time_t time_stamp)
{
	std::stringstream line;
	line << time_stamp << ",";
	for (auto value : datas)
	{
		line << value << ',';
	}
	line << "\n";
	_proc_csv << line.str();
	if (0 == _proc_line % 100)
		_proc_csv.flush();
	else
		_proc_line += 1;
}