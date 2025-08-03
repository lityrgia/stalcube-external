#pragma once
#include <cstdint>
#include <Windows.h>

using uint = unsigned int;

class Process
{
public:
	static const uintptr_t getPidByName(const wchar_t* processName);
	static HWND FindWindowByPID(DWORD pid);
	static const uintptr_t getModuleAddress(uint pid, const wchar_t* moduleName);
	//static HMODULE getRemoteModuleHandle(uint pid, const wchar_t* moduleName);
};

