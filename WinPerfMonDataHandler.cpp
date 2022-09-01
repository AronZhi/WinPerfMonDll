#include "pch.h"
#include <iostream>

#include "WinPerfMonDataHandler.h"
#include "Help.h"
#include "AmqpClient.h"
#include "AmqpChannel.h"
#include "PerfMonDataDef.h"

void WinPerfMonDataHandler::_SendData(short type)
{
	_buf.clear();
	_json_obj["machine"] = _machine_name;
	_buf = _json_obj.dump();
#ifdef SEND_TO_MQ
	AmqpChannel* channel = AmqpClient::GetInstance().Channel(1);
	int ret = 0;
	switch (type)
	{
		case 1:
		{
			ret = channel->SendMsg(_buf, "log", "perf_sys");
			break;
		}
		case 2:
		{
			ret = channel->SendMsg(_buf, "log", "perf_proc");
			break;
		}
		case 3:
		{
			ret = channel->SendMsg(_buf, "log", "perf_net");
			break;
		}
		default:
			break;
	}
#else
	std::cout << "type " << type << " " << _json_obj["test_time"] << " " << _buf << std::endl;
#endif

}

void WinPerfMonDataHandler::HandleSysData(const SysPerfData& data)
{
	_json_obj.clear();
	_json_obj["test_time"] = data.time;
	_json_obj["cpu_time"] = data.sys_cpu_time / _cpu_count;
	_json_obj["cpu_perf"] = data.sys_cpu_perf / _cpu_count;
	_json_obj["cpu_freq"] = data.sys_cpu_freq;
	_json_obj["mem"] = data.sys_mem;
	_SendData(1);
}

void WinPerfMonDataHandler::HandleNetData(const NetPerfData& data)
{
	_json_obj.clear();
	_json_obj["test_time"] = data.time;
	_json_obj["received"] = data.net_received / K_;
	_json_obj["sent"] = data.net_sent / K_;
	_SendData(3);
}

void WinPerfMonDataHandler::HandleProcData(const std::vector<ProcPerfData>& data)
{
	for (auto& item : data)
	{
		_json_obj.clear();
		_json_obj["proc_name"] = item.proc_name;
		_json_obj["test_time"] = item.time;
		_json_obj["cpu_time"] = item.proc_cpu_time / _cpu_count;
		_json_obj["thread_count"] = item.proc_thread_count;
		_json_obj["kernal_handle_count"] = item.proc_kernal_handle_Count;
		_json_obj["virtual_mem"] = item.proc_virtual_mem / M_;
		_SendData(2);
	}
}