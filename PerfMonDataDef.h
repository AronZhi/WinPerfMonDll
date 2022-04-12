#pragma once

#define M_ 1048576 // 1024*1024
#define K_ 1024
#define ERROR_PERF_VALE -100.0
#define MAX_PROC_COUNT 100

enum SysPerfField
{
	Sys_CPU_Time = 0,
	Sys_CPU_Perf,
	Sys_CPU_Freq,
	Sys_Mem,
	Sys_Total
};

enum NetPerfField
{
	Net_Received = 0,
	Net_Sent,
	Net_Total
};

enum ProcPerfField
{
	Proc_CPU_Time = 0,
	Proc_Thread_Count,
	Proc_Kernal_Handle_Count,
	Proc_Virtual_Mem,
	Proc_Total
};

struct SysPerfData
{
	uint64_t time;
	double sys_cpu_time;
	double sys_cpu_perf;
	double sys_cpu_freq;
	double sys_mem;

	SysPerfData() : time(0), sys_cpu_time(ERROR_PERF_VALE), sys_cpu_perf(ERROR_PERF_VALE), sys_cpu_freq(ERROR_PERF_VALE),
		sys_mem(ERROR_PERF_VALE) {}
};

struct NetPerfData
{
	uint64_t time;
	double net_received;
	double net_sent;

	NetPerfData() : time(0), net_received(ERROR_PERF_VALE), net_sent(ERROR_PERF_VALE) {}
};

struct ProcPerfData
{
	std::string proc_name;
	uint64_t time;
	double proc_cpu_time;
	double proc_thread_count;
	double proc_kernal_handle_Count;
	double proc_virtual_mem;
	
	ProcPerfData() : proc_name(""), time(0), proc_cpu_time(ERROR_PERF_VALE), proc_thread_count(ERROR_PERF_VALE),
		proc_kernal_handle_Count(ERROR_PERF_VALE), proc_virtual_mem(ERROR_PERF_VALE) {}
};