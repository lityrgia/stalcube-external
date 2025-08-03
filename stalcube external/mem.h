#include <stddef.h>
#include <stdint.h>
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const void* jvm;
extern HANDLE handle;

#ifdef __cplusplus
}
#endif


void readmem(void* buf, const void* addr, size_t size);
void writemem(void* buf, void* addr, size_t size);
