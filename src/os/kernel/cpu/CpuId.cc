#include <cstdint>
#include "CpuId.h"

extern "C" {
    uint32_t isCpuIdSupported();
}

bool CpuId::isSupported() {

    return isCpuIdSupported() != 0;
}
