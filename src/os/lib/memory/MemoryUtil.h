#ifndef __MemoryUtil_include__
#define __MemoryUtil_include__

#include <cstdint>

class MemoryUtil {

public:

    MemoryUtil() = delete;

    MemoryUtil(const MemoryUtil &other) = delete;

    MemoryUtil &operator=(const MemoryUtil &other) = delete;

    ~MemoryUtil() = delete;

    static uint32_t alignUp(uint32_t value, uint32_t alignment);
};

#endif
