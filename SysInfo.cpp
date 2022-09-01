#include "pch.h"
#include "SysInfo.h"

#include <IPHlpApi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <algorithm>

#include "Help.h"

bool SysInfo::_GetChildProcName(DWORD parent_pid, HANDLE hProcessSnap, std::vector<std::string>& ret_names)
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
				std::wcout << "child proc " << pe32.th32ProcessID << " / " << pe32.szExeFile << std::endl;
				WsToS(pe32.szExeFile, path);
				ret_names.push_back(path);
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	return ret_names.size() > 0 ? true : false;
}

bool SysInfo::_GetProcPid(HANDLE hProcessSnap, const std::string& exe, DWORD& ret_PID)
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

bool SysInfo::_GetProcName(HANDLE hProcessSnap, DWORD PID, std::string& name)
{
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "process snap invalid" << std::endl;
		return false;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	std::string path;
	bool find = false;
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
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

void SysInfo::_SetProceAndChildProcName(std::string& exe, std::vector<std::string>& child_names, std::vector<std::string>& ret)
{
	int index = 1;

	auto _FormatProcessName = [](std::string& exe, int index)->void {
		int count = exe.find(".exe");
		exe = exe.substr(0, count);
		index == 0 ? exe : exe = exe + "#" + std::to_string(index);
	};

	for (auto name : child_names)
	{
		if (name == exe)
			_FormatProcessName(name, index++);
		else
			_FormatProcessName(name, 0);
		ret.push_back(name);
	}
	_FormatProcessName(exe, 0);
	ret.push_back(exe);
}

bool SysInfo::GetProcAndChildProcName(const std::string& exe, std::vector<std::string>& ret)
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
		if (_GetProcPid(hProcessSnap, exe, pid))
		{
			std::vector<std::string> child_names;
			_GetChildProcName(pid, hProcessSnap, child_names);
			std::string parent(exe);
			_SetProceAndChildProcName(parent, child_names, ret);
			ret_value = true;
		}
	}
	CloseHandle(hProcessSnap);
	return ret_value;
}

bool SysInfo::GetProcAndChildProcName(DWORD pid, std::vector<std::string>& ret)
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
		if (_GetProcName(hProcessSnap, pid, exe))
		{
			std::vector<std::string> child_names;
			_GetChildProcName(pid, hProcessSnap, child_names);
			_SetProceAndChildProcName(exe, child_names, ret);
			ret_value = true;
		}
	}
	CloseHandle(hProcessSnap);
	return ret_value;
}

void SysInfo::_ListProcessModules(DWORD pid)
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

void SysInfo::_ListProcessThread(DWORD pid)
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
