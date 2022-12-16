/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_CPUID_H
#define HHUOS_CPUID_H

#include <cstdint>

#include "lib/util/memory/String.h"
#include "lib/util/data/Array.h"

namespace Util::Cpu {

class CpuId {

public:

    enum CpuType : uint8_t {
        ORIGINAL_OEM_PROCESSOR = 0x00,
        OVERDRIVE_PROCESSOR = 0x01,
        DUAL_PROCESSOR = 0x02
    };

    struct CpuInfo {
        uint8_t family;
        uint8_t model;
        uint8_t stepping;
        CpuType type;
    };

    enum CpuFeature : uint64_t {
        /* EDX features */
        FPU = 1ull << 0,
        VME = 1ull << 1,
        DE = 1ull << 2,
        PSE = 1ull << 3,
        TSC = 1ull << 4,  
        MSR = 1ull << 5,  
        PAE = 1ull << 6,  
        MCE = 1ull << 7,  
        CX8 = 1ull << 8,  
        APIC = 1ull << 9,  
        SEP = 1ull << 11, 
        MTRR = 1ull << 12, 
        PGE = 1ull << 13, 
        MCA = 1ull << 14, 
        CMOV = 1ull << 15, 
        PAT = 1ull << 16, 
        PSE36 = 1ull << 17,
        PSN = 1ull << 18, 
        CLF = 1ull << 19, 
        DTES = 1ull << 21, 
        ACPI = 1ull << 22, 
        MMX = 1ull << 23, 
        FXSR = 1ull << 24, 
        SSE = 1ull << 25, 
        SSE2 = 1ull << 26, 
        SS  = 1ull << 27, 
        HTT = 1ull << 28, 
        TM1 = 1ull << 29, 
        IA64 = 1ull << 30,
        PBE = 1ull << 31,
        /* ECX Features */
        SSE3 = 1ull << 32,
        PCLMUL = 1ull << 33,
        DTES64 = 1ull << 34,
        MONITOR = 1ull << 35,
        DS_CPL = 1ull << 36,
        VMX = 1ull << 37,
        SMX = 1ull << 38,
        EST = 1ull << 39,
        TM2 = 1ull << 40,
        SSSE3 = 1ull << 41,
        CID = 1ull << 42,
        SDBG = 1ull << 43,
        FMA = 1ull << 44,
        CX16 = 1ull << 45,
        ETPRD = 1ull << 46,
        PDCM = 1ull << 47,
        PCID = 1ull << 49,
        DCA = 1ull << 50,
        SSE4_1 = 1ull << 51,
        SSE4_2 = 1ull << 52,
        X2APIC = 1ull << 53,
        MOVBE = 1ull << 54,
        POPCNT = 1ull << 55,
        TSC_DL = 1ull << 56,
        AES = 1ull << 57,
        XSAVE = 1ull << 58,
        OSXSAVE = 1ull << 59,
        AVX = 1ull << 60,
        F16C = 1ull << 61,
        RDRAND = 1ull << 62
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    CpuId() = delete;

    /**
     * Copy Constructor.
     */
    CpuId(const CpuId &other) = delete;

    /**
     * Assignment operator.
     */
    CpuId &operator=(const CpuId &other) = delete;

    /**
     * Destructor.
     */
    ~CpuId() = default;

    [[nodiscard]] static bool isAvailable();

    [[nodiscard]] static Util::Memory::String getVendorString();

    [[nodiscard]] static uint64_t getCpuFeatureBits();

    [[nodiscard]] static Util::Data::Array<CpuFeature> getCpuFeatures();

    [[nodiscard]] static CpuInfo getCpuInfo();

    [[nodiscard]] static const char* getFeatureAsString(CpuFeature);

    static const constexpr uint32_t STEPPING_BITMASK = 0x0000000f;
    static const constexpr uint32_t MODEL_BITMASK = 0x000000f0;
    static const constexpr uint32_t FAMILY_BITMASK = 0x00000f00;
    static const constexpr uint32_t TYPE_BITMASK = 0x00003000;
    static const constexpr uint32_t EXTENDED_MODEL_BITMASK = 0x000f0000;
    static const constexpr uint32_t EXTENDED_FAMILY_BITMASK = 0x0ff00000;
};

}


#endif