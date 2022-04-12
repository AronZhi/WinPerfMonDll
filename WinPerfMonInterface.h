#ifndef WIN_PERF_MON_INTERFACE_H
#define WIN_PERF_MON_INTERFACE_H

#ifdef WINPERFMONDLL_EXPORTS
#define DLL_API __declspec(dllexport)  
#else
#define DLL_API __declspec(dllimport)  
#endif

extern "C"
{
	DLL_API bool Start(const char* json_param);
	DLL_API int Stop(void);
}


#endif // WIN_PERF_MON_INTERFACE_H