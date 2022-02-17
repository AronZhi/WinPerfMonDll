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
	Proc_CPU = 0,
	Proc_Thread_Count,
	Proc_Kernal_Handle_Count,
	Proc_Virtual_Mem,
	Proc_Total
};