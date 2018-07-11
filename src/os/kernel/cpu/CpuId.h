#ifndef __CpuId_include__
#define __CpuId_include__

#include <cstdint>

class CpuId {

public:

    CpuId() = delete;

    CpuId (const CpuId &other) = delete;

    CpuId &operator=(const CpuId &other) = delete;

    virtual ~CpuId() = delete;

    static bool isSupported();

    static uint32_t getFeatures();

    static const uint32_t FEATURE_FPU          = 1U << 0;

    static const uint32_t FEATURE_VME          = 1U << 1;

    static const uint32_t FEATURE_DE           = 1U << 2;

    static const uint32_t FEATURE_PSE          = 1U << 3;

    static const uint32_t FEATURE_TSC          = 1U << 4;

    static const uint32_t FEATURE_MSR          = 1U << 5;

    static const uint32_t FEATURE_PAE          = 1U << 6;

    static const uint32_t FEATURE_MCE          = 1U << 7;

    static const uint32_t FEATURE_CX8          = 1U << 8;

    static const uint32_t FEATURE_APIC         = 1U << 9;

    static const uint32_t FEATURE_SEP          = 1U << 11;

    static const uint32_t FEATURE_MTRR         = 1U << 12;

    static const uint32_t FEATURE_PGE          = 1U << 13;

    static const uint32_t FEATURE_MCA          = 1U << 14;

    static const uint32_t FEATURE_CMOV         = 1U << 15;

    static const uint32_t FEATURE_PAT          = 1U << 16;

    static const uint32_t FEATURE_PSE36        = 1U << 17;

    static const uint32_t FEATURE_PSN          = 1U << 18;

    static const uint32_t FEATURE_CLF          = 1U << 19;

    static const uint32_t FEATURE_DTES         = 1U << 21;

    static const uint32_t FEATURE_ACPI         = 1U << 22;

    static const uint32_t FEATURE_MMX          = 1U << 23;

    static const uint32_t FEATURE_FXSR         = 1U << 24;

    static const uint32_t FEATURE_SSE          = 1U << 25;

    static const uint32_t FEATURE_SSE2         = 1U << 26;

    static const uint32_t FEATURE_SS           = 1U << 27;

    static const uint32_t FEATURE_HTT          = 1U << 28;

    static const uint32_t FEATURE_TM1          = 1U << 29;

    static const uint32_t FEATURE_IA64         = 1U << 30;

    static const uint32_t FEATURE_PBE          = 1U << 31;
};


#endif
