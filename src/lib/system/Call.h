#ifndef HHUOS_CALL_H
#define HHUOS_CALL_H

#include <cstdint>
#include <cstdarg>
#include <lib/util/Array.h>
#include "Result.h"

namespace Standard::System {

class Call {

public:

    enum Code : uint16_t {
        SYSTEM_CALL_TEST = 0x0000,
        SCHEDULER_CREATE_THREAD = 0x0001,
        SCHEDULER_START_THREAD = 0x0002,
        SCHEDULER_YIELD = 0x0003,
        SCHEDULER_BLOCK = 0x0004
    };
    
    Call() = delete;
    
    Call(const Call &copy) = delete;
    
    Call& operator=(const Call &copy) = delete;
    
    ~Call() = delete;

    static void execute(Code code, Result &result, uint32_t paramCount...);

    
};

}

#endif
