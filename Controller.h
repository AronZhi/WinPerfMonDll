#pragma once
#include <atomic>
#include <thread>
#include <sstream>

class Controller
{
private:
	std::atomic_bool _run;
	std::thread _workThread;

	int _proc_pid;
	std::string _proc_name;
	int _sample_freq;
	std::string _net_adapter;
	std::string _task_name;
	
private:
	Controller() : _run(false), _proc_pid(0), _proc_name(""), _sample_freq(1), _net_adapter(""), _task_name(""){}
	Controller(const Controller&) = delete;
	Controller& operator =(const Controller&) = delete;

public:
	static Controller& GetInstance() {
		static Controller instance;
		return instance;
	}
	virtual ~Controller();

	bool StartLoop();
	int StopLoop();
	void Work();

	void SetProcPid(int pid) { _proc_pid = pid; }
	void SetProcName(const std::string& name) { _proc_name = name; };
	void SetSampleFrequence(int second) { _sample_freq = second; };
	void SetNetAdapter(const std::string& net_adapter) { _net_adapter = net_adapter; };
	void SetTaskName(const std::string& task_name) { _task_name = task_name; };
};

