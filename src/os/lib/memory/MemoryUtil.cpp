#include "MemoryUtil.h"

uint32_t MemoryUtil::alignUp(uint32_t value, uint32_t alignment) {

    if (alignment == 0) {

        return value;
    }

    alignment--;

    return (value + alignment) & ~alignment;
}
