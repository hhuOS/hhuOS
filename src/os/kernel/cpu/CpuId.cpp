#include <cstdint>
#include "CpuId.h"

extern "C" {
    uint32_t isCpuIdSupported();
    uint32_t getCpuFeatures();
}

bool CpuId::isSupported() {

    return isCpuIdSupported() != 0;
}

uint32_t CpuId::getFeatures() {

    return getCpuFeatures();
}
