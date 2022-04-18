#include "pch.h"
#include <iostream>

#include "WinPerfMonDataHandler.h"
#include "Help.h"
#include "AmqpClient.h"
#include "AmqpChannel.h"
#include "PerfMonDataDef.h"

void WinPerfMonDataHandler::_SendData()
{
	_buf.clear();
	_buf = _json_obj.dump();
	std::cout << _buf << std::endl;
	// AmqpChannel* channel = AmqpClient::GetInstance().Channel(1);
	// channel->SendMsg(_buf, "log", "log_info");
}

void WinPerfMonDataHandler::HandleSysData(const SysPerfData& data)
{
	_json_obj.clear();
	_json_obj["Test_Time"] = data.time;
	_json_obj["CPU_Time"] = data.sys_cpu_time;
	_json_obj["CPU_Perf"] = data.sys_cpu_perf;
	_json_obj["CPU_Freq"] = data.sys_cpu_freq;
	_json_obj["Mem"] = data.sys_mem;
	_SendData();
}

void WinPerfMonDataHandler::HandleNetData(const NetPerfData& data)
{
	_json_obj.clear();
	_json_obj["Test_Time"] = data.time;
	_json_obj["Received"] = data.net_received / K_;
	_json_obj["Sent"] = data.net_sent / K_;
	_SendData();
}

void WinPerfMonDataHandler::HandleProcData(const std::vector<ProcPerfData>& data)
{
	for (auto& item : data)
	{
		_json_obj.clear();
		_json_obj["Proc_Name"] = item.proc_name;
		_json_obj["Test_Time"] = item.time;
		_json_obj["CPU_Time"] = item.proc_cpu_time;
		_json_obj["Thread_Count"] = item.proc_thread_count;
		_json_obj["Kernal_Handle_Count"] = item.proc_kernal_handle_Count;
		_json_obj["Virtual_Mem"] = item.proc_virtual_mem / M_;
		_SendData();
	}
}