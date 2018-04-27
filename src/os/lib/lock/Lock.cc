#include "Lock.h"

extern "C" {
    uint32_t compareAndSwap(uint32_t *address, uint32_t oldValue, uint32_t newValue);
};

bool Lock::swap(uint32_t *address, uint32_t oldValue, uint32_t newValue) {

    return compareAndSwap(address, oldValue, newValue) == 1;
}