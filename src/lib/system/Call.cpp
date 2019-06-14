#include "Call.h"

namespace Standard::System {

void Call::execute(Call::Code code, Result &result, uint32_t paramCount, ...) {
    va_list args;
    va_start(args, paramCount);

    auto eaxValue = static_cast<uint32_t>(code | (paramCount << 16u));
    auto ebxValue = reinterpret_cast<uint32_t>(args);
    auto ecxValue = reinterpret_cast<uint32_t>(&result);

    asm volatile (
            "movl %0, %%eax;"
            "movl %1, %%ebx;"
            "movl %2, %%ecx;"
            "int $0x80;"
            : :
            "r"(eaxValue),
            "r"(ebxValue),
            "r"(ecxValue));

    va_end(args);
}

}
