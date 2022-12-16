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

#include "asm_interface.h"
#include "lib/util/data/ArrayList.h"
#include "CpuId.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Util::Cpu {

bool CpuId::isAvailable() {
    return is_cpuid_available() != 0;
}

Util::Memory::String CpuId::getVendorString() {
    if (!isAvailable()) {
        return "";
    }

    uint32_t vendor[3];
    asm volatile(
    "mov $0,%%eax;"
    "cpuid;"
    "mov %%ebx,%0;"
    "mov %%edx,%1;"
    "mov %%ecx,%2;"
    : "=r"(vendor[0]), "=r"(vendor[1]), "=r"(vendor[2])
    :
    : "%ebx", "%ecx", "%edx"
    );

    if (reinterpret_cast<uint8_t*>(vendor)[0] == 0) {
        return "";
    }

    return { reinterpret_cast<uint8_t*>(vendor), 12 };
}

uint64_t CpuId::getCpuFeatureBits() {
    if (!isAvailable()) {
        return 0;
    }

    auto features = Util::Data::ArrayList<CpuFeature>();

    uint32_t ecx, edx;
    asm volatile(
            "mov $1,%%eax;"
            "cpuid;"
            "mov %%edx,%0;"
            "mov %%ecx,%1;"
            : "=r"(edx), "=r"(ecx)
            :
            : "%ecx", "%edx"
            );

    return static_cast<uint64_t>(ecx) << 32 | edx;
}

Util::Data::Array<CpuId::CpuFeature> CpuId::getCpuFeatures() {
    if (!isAvailable()) {
        return Util::Data::Array<CpuId::CpuFeature>(0);
    }

    auto features = Util::Data::ArrayList<CpuFeature>();
    auto featureBits = getCpuFeatureBits();

    uint64_t i;
    for (i = 1; i <= CpuFeature::AVX; i *= 2) {
        if ((featureBits & i) != 0) {
            features.add(static_cast<CpuFeature>(i));
        }
    }

    return features.toArray();
}

CpuId::CpuInfo CpuId::getCpuInfo() {
    if (!isAvailable()) {
        return {};
    }

    uint32_t eax;
    asm volatile(
    "mov $1,%%eax;"
    "cpuid;"
    "mov %%eax,%0;"
    : "=r"(eax)
    :
    : "%eax"
    );

    uint8_t extendedModel = (eax & EXTENDED_MODEL_BITMASK) >> 16;
    uint8_t extendedFamily = (eax & EXTENDED_FAMILY_BITMASK) >> 20;
    uint8_t type = (eax & FAMILY_BITMASK) >> 12;
    uint8_t family = extendedFamily + ((eax & FAMILY_BITMASK) >> 8);
    uint8_t model = (extendedModel << 4) + ((eax & MODEL_BITMASK) >> 4);
    uint8_t stepping = eax & STEPPING_BITMASK;

    return { family, model, stepping, static_cast<CpuType>(type) };
}

const char *CpuId::getFeatureAsString(CpuId::CpuFeature feature) {
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
            return "UNKNOWN";
    }
}

}