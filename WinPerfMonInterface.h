#ifndef WIN_PERF_MON_INTERFACE_H
#define WIN_PERF_MON_INTERFACE_H

#ifdef WINPERFMONDLL_EXPORTS
#define DLL_API __declspec(dllexport)  
#else
#define DLL_API __declspec(dllimport)  
#endif

extern "C"
{
	DLL_API void SetProcName(const char* name);
	DLL_API void SetProcPid(int Pid);
	DLL_API void SetNetAdapter(const char* name);
	DLL_API bool Start(void);
	DLL_API void Stop(void);
}


#endif // WIN_PERF_MON_INTERFACE_H