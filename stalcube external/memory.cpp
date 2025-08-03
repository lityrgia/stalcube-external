#include "memory.h"
#include "Hijack Handle.h"

bool Memory::init(uintptr_t pid) {
	//processHandle = HijackExistingHandle(pid);
	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	return processHandle != NULL;
}

const std::uintptr_t Memory::findDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t address = ptr;

	for (unsigned int i = 0; i < offsets.size(); ++i) {
		BOOL rpmReturn = ReadProcessMemory(processHandle, (LPVOID)address, &address, sizeof(address), 0);

		address += offsets[i];
	}

	return address;
}

void Memory::readRaw(const std::uintptr_t& address, char* buffer, size_t size)
{
	::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), buffer, size, NULL);
}