#include <cstdint>
#include <cstdarg>
#include "lib/util/base/operators.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/FreeListMemoryManager.h"

// Export functions
extern "C" {
void initMemoryManager(uint32_t, uint32_t);
void _exit(int32_t);
}

void initMemoryManager(uint32_t startAddress, uint32_t endAddress) {
    auto *memoryManager = new (reinterpret_cast<void*>(Util::USER_SPACE_MEMORY_MANAGER_ADDRESS)) Util::FreeListMemoryManager();
    memoryManager->initialize(startAddress, endAddress);
}

uint16_t systemCall(uint16_t code, uint32_t paramCount...) {
    va_list args;
    va_start(args, paramCount);
    uint16_t result;

    auto eaxValue = static_cast<uint32_t>(code | (paramCount << 16u));
    auto ebxValue = reinterpret_cast<uint32_t>(args);
    auto ecxValue = reinterpret_cast<uint32_t>(&result);

    asm volatile (
    "movl %0, %%eax;"
    "movl %1, %%ebx;"
    "movl %2, %%ecx;"
    "int $0x86;"
    : :
    "r"(eaxValue),
    "r"(ebxValue),
    "r"(ecxValue));

    va_end(args);
    return result;
}

void _exit(int32_t exitCode) {
    systemCall(1, 1, exitCode);
}