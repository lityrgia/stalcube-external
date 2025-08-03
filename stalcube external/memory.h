#pragma once
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <algorithm>

class Memory
{
public:
	uintptr_t processId = 0;
	HANDLE processHandle = nullptr;

public:

	Memory() {}
	~Memory()
	{
		if (processHandle)
			::CloseHandle(processHandle);
	}

	bool init(uintptr_t pid);
	const uintptr_t findDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
	void readRaw(const std::uintptr_t& address, char* buffer, size_t size);

    inline bool isRegionReadable(const MEMORY_BASIC_INFORMATION& mbi) {
        return (mbi.State == MEM_COMMIT) &&
            (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) &&
            !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD));
    }

    uintptr_t scanRegion(uintptr_t base, size_t size, const std::vector<uint8_t>& pattern, const std::string& mask) {
        const size_t patternSize = pattern.size();
        if (patternSize != mask.size() || patternSize == 0 || size < patternSize)
            return 0;

        std::vector<uint8_t> buffer(size);
        SIZE_T bytesRead = 0;

        if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(base), buffer.data(), size, &bytesRead))
            return 0;

        for (size_t i = 0; i <= bytesRead - patternSize; ++i) {
            bool match = true;

            for (size_t j = 0; j < patternSize; ++j) {
                if (mask[j] != '?' && buffer[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                return base + i;
            }
        }

        return 0;
    }

    uintptr_t findPattern(const std::vector<uint8_t>& pattern, const std::string& mask) {
        if (!processHandle)
            return 0;

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        uintptr_t minAddr = reinterpret_cast<uintptr_t>(sysInfo.lpMinimumApplicationAddress);
        uintptr_t maxAddr = reinterpret_cast<uintptr_t>(sysInfo.lpMaximumApplicationAddress);

        MEMORY_BASIC_INFORMATION mbi{};
        uintptr_t currentAddr = minAddr;

        while (currentAddr < maxAddr) {
            if (!VirtualQueryEx(processHandle, reinterpret_cast<LPCVOID>(currentAddr), &mbi, sizeof(mbi)))
                break;

            uintptr_t regionStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
            uintptr_t regionEnd = regionStart + mbi.RegionSize;
            currentAddr = regionEnd;

            if (isRegionReadable(mbi)) {
                uintptr_t foundAddr = scanRegion(regionStart, mbi.RegionSize, pattern, mask);
                if (foundAddr)
                    return foundAddr;
            }
        }

        return 0;
    }

    template<typename T = uintptr_t>
    T getAbsoluteAddress(uintptr_t addy, int offset) {
        int32_t relOffset = read<int32_t>(addy + offset);
        return static_cast<T>(addy + offset + sizeof(int32_t) + relOffset);
    }

	template <typename T>
	constexpr const T read(const std::uintptr_t& address)
	{
		T value = { };
		::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	constexpr void write(const std::uintptr_t& address, const T& value)
	{
		::WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}
};

