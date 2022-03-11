#include "pch.h"

#include "WinPerfMonDataHandler.h"

WinPerfMonDataHandler::WinPerfMonDataHandler(const std::string& local_file_name) 
	: _is_send_metric(false), _local_backup(local_file_name)
{

}

void WinPerfMonDataHandler::HandleSysData(const std::array<double, Sys_Total>& datas, time_t time_stamp)
{
	_local_backup.WriteSysPrefLine(datas, time_stamp);
}

void WinPerfMonDataHandler::HandleNetData(const std::array<double, Net_Total>& datas, time_t time_stamp)
{
	_local_backup.WriteNetPrefLine(datas, time_stamp);
}

void WinPerfMonDataHandler::HandleProcData(const std::array<double, Proc_Total>& datas, time_t time_stamp)
{
	_local_backup.WriteProcPrefLine(datas, time_stamp);
}