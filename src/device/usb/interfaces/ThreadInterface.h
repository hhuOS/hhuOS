#ifndef ThreadInterface__include
#define ThreadInterface__include

#ifdef __cplusplus

#include <cstdint>

extern "C"{
#endif

void mdelay(uint32_t millis);
void udelay(uint32_t micros);
void ndelay(uint32_t nanos);
void yield_c();

#ifdef __cplusplus
}
#endif

#endif     