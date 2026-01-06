/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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


#include "lib/util/collection/ArrayList.h"
#include "CpuId.h"

namespace Util {
namespace Hardware {
namespace CpuId {

constexpr uint32_t STEPPING_BITMASK = 0x0000000f;
constexpr uint32_t MODEL_BITMASK = 0x000000f0;
constexpr uint32_t FAMILY_BITMASK = 0x00000f00;
constexpr uint32_t TYPE_BITMASK = 0x00003000;
constexpr uint32_t EXTENDED_MODEL_BITMASK = 0x000f0000;
constexpr uint32_t EXTENDED_FAMILY_BITMASK = 0x0ff00000;

bool isAvailable() {
    uint32_t eax;

    asm volatile(
    "pushf;"                    // Save original EFLAGS
    "pushf;"                    // Store EFLAGS
    "xorl $0x00200000,(%%esp);" // Invert the ID bit in stored EFLAGS
    "popf;"                     // Load stored EFLAGS (with ID bit inverted)
    "pushf;"                    // Store EFLAGS again (ID bit may or may not be inverted)
    "pop %%eax;"                // eax = modified EFLAGS (ID bit may or may not be inverted)
    "xor (%%esp),%%eax;"        // eax = whichever bits were changed
    "popf;"                     // Restore original EFLAGS
    "and $0x00200000,%%eax;"     // eax = zero if ID bit can't be changed, else non-zero
    : "=a"(eax)
    );

    return eax == 0x00200000; // Check if the ID bit is set
}

String getVendorString() {
    if (!isAvailable()) {
        return "";
    }

    uint32_t vendor[3];
    asm volatile (
    "mov $0,%%eax;"
    "cpuid;"
    : "=b"(vendor[0]), "=c"(vendor[1]), "=d"(vendor[2])
    :
    : "%eax"
    );

    return { reinterpret_cast<uint8_t*>(vendor), 12 };
}


CpuInfo getCpuInfo() {
    if (!isAvailable()) {
        return {};
    }

    uint32_t eax, ecx, edx;
    asm volatile (
    "mov $1,%%eax;"
    "cpuid;"
    : "=a"(eax), "=c"(ecx), "=d"(edx)
    );

    uint8_t extendedModel = (eax & EXTENDED_MODEL_BITMASK) >> 16;
    uint8_t extendedFamily = (eax & EXTENDED_FAMILY_BITMASK) >> 20;
    uint8_t type = (eax & FAMILY_BITMASK) >> 12;
    uint8_t family = extendedFamily + ((eax & FAMILY_BITMASK) >> 8);
    uint8_t model = (extendedModel << 4) + ((eax & MODEL_BITMASK) >> 4);
    uint8_t stepping = eax & STEPPING_BITMASK;
    uint64_t features = static_cast<uint64_t>(ecx) << 32 | edx;

    return { family, model, stepping, static_cast<CpuType>(type), features };
}

Array<CpuFeature> CpuInfo::getFeaturesAsArray() const {
    if (!isAvailable()) {
        return Util::Array<CpuFeature>(0);
    }

    ArrayList<CpuFeature> features{};
    for (uint64_t i = 1; i <= AVX; i *= 2) {
        if ((CpuInfo::features & i) != 0) {
            features.add(static_cast<CpuFeature>(i));
        }
    }

    return features.toArray();
}

const char* getFeatureAsString(const CpuFeature feature) {
    switch (feature) {
        case FPU:
            return "FPU";
        case VME:
            return "VME";
        case DE:
            return "DE";
        case PSE:
            return "PSE";
        case TSC:
            return "TSC";
        case MSR:
            return "MSR";
        case PAE:
            return "PAE";
        case MCE:
            return "MCE";
        case CX8:
            return "CX8";
        case APIC:
            return "APIC";
        case SEP:
            return "SEP";
        case MTRR:
            return "MTRR";
        case PGE:
            return "PGE";
        case MCA:
            return "MCA";
        case CMOV:
            return "CMOV";
        case PAT:
            return "PAT";
        case PSE36:
            return "PSE36";
        case PSN:
            return "PSN";
        case CLF:
            return "CLF";
        case DTES:
            return "DTES";
        case ACPI:
            return "ACPI";
        case MMX:
            return "MMX";
        case FXSR:
            return "FXSR";
        case SSE:
            return "SSE";
        case SSE2:
            return "SSE2";
        case SS:
            return "SS";
        case HTT:
            return "HTT";
        case TM1:
            return "TM1";
        case IA64:
            return "IA64";
        case PBE:
            return "PBE";
        case PCLMUL:
            return "PCLMUL";
        case SSE3:
            return "SSE3";
        case DTES64:
            return "DTES64";
        case MONITOR:
            return "MONITOR";
        case DS_CPL:
            return "DS_CPL";
        case VMX:
            return "VMX";
        case SMX:
            return "SMX";
        case EST:
            return "EST";
        case TM2:
            return "TM2";
        case SSSE3:
            return "SSSE3";
        case CID:
            return "CID";
        case SDBG:
            return "SDBG";
        case FMA:
            return "FMA";
        case CX16:
            return "CX16";
        case ETPRD:
            return "ETPRD";
        case PDCM:
            return "PDCM";
        case PCID:
            return "PCID";
        case DCA:
            return "DCA";
        case SSE4_1:
            return "SSE4_1";
        case SSE4_2:
            return "SSE4_2";
        case X2APIC:
            return "X2APIC";
        case MOVBE:
            return "MOVBE";
        case POPCNT:
            return "POPCNT";
        case TSC_DL:
            return "TSC_DL";
        case AES:
            return "AES";
        case XSAVE:
            return "XSAVE";
        case OSXSAVE:
            return "OSXSAVE";
        case AVX:
            return "AVX";
        case F16C:
            return "F16C";
        case RDRAND:
            return "RDRAND";
        default:
            Panic::fire(Panic::INVALID_ARGUMENT, "CpuId: Invalid CPU feature!");
    }
}

}
}
}