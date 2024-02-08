#ifndef ThreadInterface__include
#define ThreadInterface__include

#ifdef __cplusplus

#include <cstdint>

extern "C"{
#endif

#include "../controller/UsbController.h"

void mdelay(uint32_t millis);
void udelay(uint32_t micros);
void ndelay(uint32_t nanos);
void yield_c();
void create_thread(char* name, UsbController* controller);

#ifdef __cplusplus
}
#endif

#endif     