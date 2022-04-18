#include "pch.h"
#include "WinPerfMonitor.h"

#include <iostream>
#include <array>

#include "WinPerfCounter.h"
#include "PerfMonDataDef.h"
#include "WinPerfMonDataHandler.h"
#include "nlohmann/json.hpp"
#include "AmqpClient.h"

WinPerfMonitor::~WinPerfMonitor()
{
	Stop();
}

bool WinPerfMonitor::Start(std::string json_param)
{
	try
	{
		if (_run)
			return false;
		else
			_run = true;

		AmqpClient::GetInstance().Login("10.224.78.182", 5672, "tatest", "P@ss1234", "test");

		auto Work = [&](std::string json_param){
			nlohmann::json param = nlohmann::json::parse(json_param);
			WinPerfMonDataHandler handler;
			WinPerfCounter counter;
			counter.AddSysCounter();
			for (auto& el : param.items())
			{
				if (el.key() == "sample_interval")
					counter.SetSampleInterval(el.value().get<int>());
				else if (el.key() == "net_adapter")
					counter.AddNetCounter(el.value().get<std::string>());
				else if (el.key() == "process")
					counter.AddProcessCounter(el.value().get<std::string>());
				else if (el.key() == "pid")
					counter.AddProcessCounter(el.value().get<DWORD>());
			}
			SysPerfData sys_data;
			NetPerfData net_data;
			std::vector<ProcPerfData> proc_data;
			std::cout << "start loop" << std::endl;
			while(_run)
			{
				counter.BlockCollectData();
				counter.GetSysPref(sys_data);
				counter.GetNetPref(net_data);
				counter.GetProcPref(proc_data);
				handler.HandleSysData(sys_data);
				handler.HandleNetData(net_data);
				handler.HandleProcData(proc_data);
			}
		};

		_workThread = std::thread(Work, json_param);
		return true;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return false;
}

int WinPerfMonitor::Stop()
{
	int ret = 0;
	try
	{
		if (_run)
		{
			_run = false;
			_workThread.join();
			std::cout << "stop monitor" << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "stop thread fail, please restart process" << std::endl;
		ret = -1;
	}
	AmqpClient::GetInstance().Logout();
	return ret;
}
