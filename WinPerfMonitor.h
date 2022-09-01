#pragma once

#include <atomic>
#include <thread>
#include <string>

class WinPerfMonitor
{
private:
	std::atomic_bool _run;
	std::thread _workThread;

private:
	WinPerfMonitor() : _run(true) {}
	WinPerfMonitor(const WinPerfMonitor&) = delete;
	WinPerfMonitor& operator =(const WinPerfMonitor&) = delete;

public:
	static WinPerfMonitor& GetInstance() {
		static WinPerfMonitor instance;
		return instance;
	}
	virtual ~WinPerfMonitor();

	bool Start(std::string json_param);
	int Stop();
};
