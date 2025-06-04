/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_HARDWARE_SMBIOS_H
#define HHUOS_LIB_UTIL_HARDWARE_SMBIOS_H

#include <stdint.h>
#include <stddef.h>

/// Structures and definitions for the System Management BIOS (SMBIOS).
/// In user space, the SMBIOS structures are accessible via the '/device/smbios' directory.
namespace Util::Hardware::SmBios {

/// Different types SMBIOS table headers can have.
enum HeaderType : uint8_t {
    BIOS_INFORMATION = 0,
    SYSTEM_INFORMATION = 1,
    BASEBOARD_INFORMATION = 2,
    CHASSIS_INFORMATION = 3,
    PROCESSOR_INFORMATION = 4,
    MEMORY_CONTROLLER_INFORMATION = 5,
    MEMORY_MODULE_INFORMATION = 6,
    CACHE_INFORMATION = 7,
    PORT_CONNECTOR_INFORMATION = 8,
    SYSTEM_SLOTS_INFORMATION = 9,
    ONBOARD_DEVICES_INFORMATION = 10,
    OEM_STRINGS = 11,
    SYSTEM_CONFIGURATION_OPTIONS = 12,
    BIOS_LANGUAGE_INFORMATION = 13,
    GROUP_ASSOCIATIONS = 14,
    SYSTEM_EVENT_LOG = 15,
    PHYSICAL_MEMORY_ARRAY = 16,
    MEMORY_DEVICE_INFORMATION = 17,
    MEMORY_ERROR_INFORMATION_32_BIT = 18,
    MEMORY_ARRAY_MAPPED_ADDRESS = 19,
    MEMORY_DEVICE_MAPPED_ADDRESS = 20,
    BUILT_IN_POINTING_DEVICE = 21,
    PORTABLE_BATTERY = 22,
    SYSTEM_RESET = 23,
    HARDWARE_SECURITY = 24,
    SYSTEM_POWER_CONTROLS = 25,
    VOLTAGE_PROBE = 26,
    COOLING_DEVICE = 27,
    TEMPERATURE_PROBE = 28,
    ELECTRICAL_CURRENT_PROBE = 29,
    OUT_OF_BAND_REMOTE_ACCESS = 30,
    BOOT_INTEGRITY_SERVICES_ENTRY_POINT = 31,
    SYSTEM_BOOT_INFORMATION = 32,
    MEMORY_ERROR_INFORMATION_64_BIT = 33,
    MANAGEMENT_DEVICE = 34,
    MANAGEMENT_DEVICE_COMPONENT_DATA = 35,
    MANAGEMENT_DEVICE_THRESHOLD_DATA = 36,
    MEMORY_CHANNEL = 37,
    IPMI_DEVICE_INFORMATION = 38,
    SYSTEM_POWER_SUPPLY = 39,
    ADDITIONAL_INFORMATION = 40,
    ONBOARD_DEVICES_EXTENDED_INFORMATION = 41,
    MANAGEMENT_CONTROLLER_HOST_INTERFACE = 42,
    TPM_DEVICE = 43,
    PROCESSOR_ADDITIONAL_INFORMATION = 44,
    FIRMWARE_INVENTORY_INFORMATION = 45,
    STRING_PROPERTY = 46,
    INACTIVE = 126,
    END_OF_TABLE = 127
};

/// Header for SMBIOS tables.
/// The tables are located directly after each other in memory, all beginning with a header.
/// After each table, there may be a variable number of strings, which are referenced by the table.
/// The `length` field in the header indicates the length of the table, including the header itself,
/// but excluding the strings. To get the full length of the table including the strings,
/// call `calculateFullLength()` on the header.
/// In user space, the SMBIOS tables are accessible via the `/device/smbios/tables` directory.
struct TableHeader {
    HeaderType type;
    uint8_t length;
    uint16_t handle;

    /// Calculate the full length of the table, including the strings.
    [[nodiscard]] size_t calculateFullLength() const;

    /// Calculate the number of strings in the table.
    [[nodiscard]] size_t calculateStringCount() const;

    /// Get the string at the given index from the string table.
    [[nodiscard]] const char* getString(size_t number) const;
} __attribute__ ((packed));

/// The BIOS Information table provides information about the BIOS firmware.
struct BiosInformation {
    TableHeader header;
    uint8_t vendorString;
    uint8_t versionString;
    uint16_t startAddressSegment;
    uint8_t releaseDateString;
    uint8_t romSize;
    uint64_t characteristics;
    uint8_t characteristicsExtension1;
    uint8_t characteristicsExtension2;
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint8_t embeddedControllerMajorVersion;
    uint8_t embeddedControllerMinorVersion;
    uint16_t extendedRomSize;

    /// Get the vendor name of the BIOS.
    [[nodiscard]] const char* getVendorName() const;

    /// Get the version string of the BIOS.
    [[nodiscard]] const char* getVersion() const;

    /// Get the release date string of the BIOS.
    [[nodiscard]] const char* getReleaseDate() const;

    /// Calculate the size of the runtime BIOS image in bytes.
    /// That is the amount of bytes that the BIOS occupies in the system memory.
    [[nodiscard]] size_t calculateRuntimeSize() const;

    /// Calculate the size of the ROM image in bytes.
    [[nodiscard]] size_t calculateRomSize() const;
} __attribute__ ((packed));

}

#endif
