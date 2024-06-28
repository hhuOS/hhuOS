#ifndef TIME_INTERFACE__INCLUDE
#define TIME_INTERFACE__INCLUDE

#ifdef __cplusplus

#include <cstdint>

extern "C"{
#endif

uint32_t getSystemTimeInMilli();

uint32_t getSystemTimeInMicro();

uint32_t getSystemTimeInNano();

uint32_t addMilis(uint32_t time_in_millis, uint32_t add_time_millis);

#ifdef __cplusplus
}
#endif

#endif
