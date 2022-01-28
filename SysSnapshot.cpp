#include "pch.h"
#include "SysSysSnapshot.h"

#include <IPHlpApi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <algorithm>

#include "Help.h"

bool SysSnapshot::GetNetAdapterName(std::string& ret)
{
	ULONG ulSize = 0;
	IP_ADAPTER_INFO *pAdapter = nullptr;
	PDH_STATUS res = GetAdaptersInfo(pAdapter, &ulSize);
	if (ERROR_BUFFER_OVERFLOW == res)
	{
		pAdapter = new IP_ADAPTER_INFO[ulSize];
		res = GetAdaptersInfo(pAdapter, &ulSize);
	}
	if (ERROR_SUCCESS != res)
	{
		std::cout << "Get Adapters Info fail" << std::endl;
		return false;
	}
	IPAddr ipAddr = { 0 };
	DWORD dwIndex = -1;
	DWORD nRet = GetBestInterface(ipAddr, &dwIndex);
	if (NO_ERROR != nRet)
		std::cout << "Get Best Interface fail: " << nRet << std::endl;
	bool find = false;
	for (auto *pCur = pAdapter; pCur != NULL; pCur = pCur->Next) {
		if (pCur->Index == dwIndex) {
			ret = pCur->Description;
			std::replace(ret.begin(), ret.end(), '(', '[');
			std::replace(ret.begin(), ret.end(), ')', ']');
			std::cout << std::endl << "Adapter Descrip: " << ret << std::endl;
			find = true;
			break;
		}
	}
	delete[] pAdapter;
	return find;
}

bool SysSnapshot::GetChildProcName(DWORD parent_pid, HANDLE hProcessSnap, std::vector<std::string>& ret_names)
{
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	std::string path;
	bool find = false;
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			dwPriorityClass = 0;
			if (pe32.th32ParentProcessID == parent_pid)
			{
				std::wcout << "child proc " <<pe32.th32ProcessID << " / " << pe32.szExeFile << std::endl;
				WsToS(pe32.szExeFile, path);
				ret_names.push_back(path);
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	return ret_names.size() > 0 ? true : false;
}

bool SysSnapshot::GetProcPid(HANDLE hProcessSnap, const std::string& exe, DWORD& ret_PID)
{
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "process snap invalid" << std::endl;
		return false;
	}
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	std::string path;
	bool find = false;
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			dwPriorityClass = 0;
			WsToS(pe32.szExeFile, path);
			if (path == exe)
			{
				ret_PID = pe32.th32ProcessID;
				find = true;
				break;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	return find;
}

bool SysSnapshot::GetProcName(HANDLE hProcessSnap, DWORD PID, std::string& name)
{
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "process snap invalid" << std::endl;
		return false;
	}
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	std::string path;
	bool find = false;
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			dwPriorityClass = 0;
			WsToS(pe32.szExeFile, path);
			if (pe32.th32ProcessID == PID)
			{
				WsToS(pe32.szExeFile, name);
				find = true;
				break;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	return find;
}

void SysSnapshot::formatProcessName(std::string& exe, int index)
{
	int count = exe.find(".exe");
	exe = exe.substr(0, count);
	index == 0 ? exe : exe = exe + "#" + std::to_string(index);
}

void SysSnapshot::SetProceAndChildProcName(std::string& exe, std::vector<std::string>& child_names, std::vector<std::string>& ret)
{
	int index = 1;
	for (auto name : child_names)
	{
		if (name == exe)
		{
			formatProcessName(name, index++);
		}
		else
		{
			formatProcessName(name, 0);
		}
		ret.push_back(name);
	}
	formatProcessName(exe, 0);
	ret.push_back(exe);
}

bool SysSnapshot::GetProcAndChildProcName(const std::string& exe, std::vector<std::string>& ret)
{
	// Take a snapshot of all processes in the system.
	bool ret_value = false;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreateToolhelp32Snapshot (of processes)" << std::endl;
	}
	else
	{
		DWORD pid = 0;
		if (GetProcPid(hProcessSnap, exe, pid))
		{
			std::vector<std::string> child_names;
			GetChildProcName(pid, hProcessSnap, child_names);
			std::string parent(exe);
			SetProceAndChildProcName(parent, child_names, ret);
			ret_value = true;
		}
	}
	CloseHandle(hProcessSnap);
	return ret_value;
}

bool SysSnapshot::GetProcAndChildProcName(DWORD pid, std::vector<std::string>& ret)
{
	bool ret_value = false;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreateToolhelp32Snapshot (of processes)" << std::endl;
	}
	else
	{
		std::string exe("");
		if (GetProcName(hProcessSnap, pid, exe))
		{
			std::vector<std::string> child_names;
			GetChildProcName(pid, hProcessSnap, child_names);
			SetProceAndChildProcName(exe, child_names, ret);
			ret_value = true;
		}
	}
	CloseHandle(hProcessSnap);
	return ret_value;
}

void SysSnapshot::ListProcessModules(DWORD pid)
{
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreateToolhelp32Snapshot (of modules)" << std::endl;
	}
	else
	{
		MODULEENTRY32 me32;
		me32.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(hModuleSnap, &me32))
		{
			std::cout << "Module32First fail" << std::endl;
		}
		else
		{
			do
			{
				_tprintf(TEXT("\n\n     MODULE NAME:     %s"), me32.szModule);
				_tprintf(TEXT("\n     Executable     = %s"), me32.szExePath);
				_tprintf(TEXT("\n     Process ID     = 0x%08X"), me32.th32ProcessID);
				_tprintf(TEXT("\n     Ref count (g)  = 0x%04X"), me32.GlblcntUsage);
				_tprintf(TEXT("\n     Ref count (p)  = 0x%04X"), me32.ProccntUsage);
				_tprintf(TEXT("\n     Base address   = 0x%08X"), (DWORD)me32.modBaseAddr);
				_tprintf(TEXT("\n     Base size      = %d"), me32.modBaseSize);

			} while (Module32Next(hModuleSnap, &me32));
		}
	}
	CloseHandle(hModuleSnap);
}

void SysSnapshot::ListProcessThread(DWORD pid)
{
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	if (INVALID_HANDLE_VALUE == hThreadSnap)
	{
		std::cout << "CreateToolhelp32Snapshot (of threads)" << std::endl;
	}
	else
	{
		THREADENTRY32 te32;
		te32.dwSize = sizeof(THREADENTRY32);
		if (!Thread32First(hThreadSnap, &te32))
		{
			std::cout << "Thread32First fail" << std::endl; // show cause of failure
		}
		else
		{
			do
			{
				_tprintf(TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
				_tprintf(TEXT("\n     Base priority  = %d"), te32.tpBasePri);
				_tprintf(TEXT("\n     Delta priority = %d"), te32.tpDeltaPri);
				_tprintf(TEXT("\n"));
			} while (Thread32Next(hThreadSnap, &te32));
		}
	}
	CloseHandle(hThreadSnap);
}