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

#ifndef HHUOS_LIB_UTIL_HARDWARE_CPUID_H
#define HHUOS_LIB_UTIL_HARDWARE_CPUID_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/collection/Array.h"

namespace Util {
namespace Hardware {

/// Contains functions to retrieve information about the CPU using the CPUID instruction.
namespace CpuId {

/// Enumeration of CPU types, used in the `CpuInfo` structure.
enum CpuType : uint8_t {
    ORIGINAL_OEM_PROCESSOR = 0x00,
    OVERDRIVE_PROCESSOR = 0x01,
    DUAL_PROCESSOR = 0x02
};

/// Enumeration of CPU features, represented as bit flags.
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

/// Structure to hold CPU information returned by the CPUID instruction with EAX = 1.
/// Call `getCpuInfo()` to retrieve this information.
struct CpuInfo {
    /// CPU family.
    uint8_t family;
    /// CPU model.
    uint8_t model;
    /// CPU stepping (revision).
    uint8_t stepping;
    /// CPU type, as defined by the `CpuType` enum.
    CpuType type;
    /// CPU feature bitmask, as defined by the `CpuFeature` enum.
    uint64_t features;

    /// Get the CPU features as an array of `CpuFeature` enums.
    /// This makes it easy to iterate over the features or check for specific ones.
    ///
    /// ### Example
    /// ```c++
    /// if (Util::Hardware::CpuId::isAvailable()) {
    ///     const auto cpuInfo = Util::Hardware::CpuId::getCpuInfo(); // Retrieve CPU information
    ///
    ///     // Iterate over the CPU features and print them
    ///     Util::System::out << "CPU Features: ";
    ///     for (const auto &feature : cpuInfo.getFeaturesAsArray()) {
    ///         Util::System::out << Util::Hardware::CpuId::getFeatureAsString(feature) << " ";
    ///     }
    ///     Util::System::out << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// }
    /// ```
    Array<CpuFeature> getFeaturesAsArray() const;
};

/// Check if the CPUID instruction is available on the current CPU.
/// This is done by checking if the ID bit (bit 21) in the EFLAGS register can be changed.
///
/// The code for this function is based on a code snippet from https://wiki.osdev.org/CPUID
bool isAvailable();

/// Get the 12 byte vendor string of the CPU by executing the CPUID instruction with EAX = 0.
///
/// ### Example
/// ```c++
/// if (Util::Hardware::CpuId::isAvailable()) {
///     const auto vendor = Util::Hardware::CpuId::getVendorString();
///     Util::System::out << "CPU Vendor: " << vendor << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
/// }
/// ```
String getVendorString();

/// Get CPU information by executing the CPUID instruction with EAX = 1.
///
/// ### Example
/// ```c++
/// if (Util::Hardware::CpuId::isAvailable()) {
///     const auto cpuInfo = Util::Hardware::CpuId::getCpuInfo();
///     Util::System::out << "CPU Family: " << cpuInfo.family
///         << ", Model: " << cpuInfo.model
///         << ", Stepping: " << cpuInfo.stepping
///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
/// }
/// ```
CpuInfo getCpuInfo();

/// Get a string representation of the specified CPU feature.
const char *getFeatureAsString(CpuFeature feature);

}

}
}

#endif
