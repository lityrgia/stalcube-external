#include "process.h"

#include <TlHelp32.h>

const uintptr_t Process::getPidByName(const wchar_t* processName) {
	::PROCESSENTRY32 entry = { };
	entry.dwSize = sizeof(::PROCESSENTRY32);

	const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	while (::Process32Next(snapShot, &entry))
	{
		if (!_wcsicmp(entry.szExeFile, processName))
		{
			return static_cast<uintptr_t>(entry.th32ProcessID);
			break;
		}
	}

	return 0;
}

HWND Process::FindWindowByPID(DWORD pid) {
	HWND hwnd = GetTopWindow(NULL);
	while (hwnd) {
		DWORD windowPid;
		GetWindowThreadProcessId(hwnd, &windowPid);

		if (windowPid == pid && IsWindowVisible(hwnd)) {
			return hwnd;
		}

		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
	}
	return nullptr;
}

const uintptr_t Process::getModuleAddress(uint pid, const wchar_t* moduleName)
{
	::MODULEENTRY32 entry = { };
	entry.dwSize = sizeof(::MODULEENTRY32);

	const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

	uintptr_t result = 0;

	while (::Module32Next(snapShot, &entry))
	{
		if (!_wcsicmp(entry.szModule, moduleName))
		{
			result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			break;
		}
	}

	if (snapShot)
		::CloseHandle(snapShot);

	return result;
}

//HMODULE Process::getRemoteModuleHandle(uint pid, const wchar_t* moduleName)
//{
//	::MODULEENTRY32 entry = { };
//	entry.dwSize = sizeof(::MODULEENTRY32);
//
//	const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
//
//	while (::Module32Next(snapShot, &entry))
//	{
//		if (!_wcsicmp(entry.szModule, moduleName))
//		{
//			return entry.hModule;
//		}
//	}
//
//	if (snapShot)
//		::CloseHandle(snapShot);
//
//	return nullptr;
//}