#include "SystemCall.h"

extern "C" int atexit (void (*func)(void)) noexcept;

int atexit (void (*func)(void)) {
    return 0;
}
