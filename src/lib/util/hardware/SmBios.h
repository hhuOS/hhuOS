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

#include "collection/Array.h"
#include "collection/Iterator.h"

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
    /// Header type as defined by the `HeaderType` enum.
    HeaderType type;
    /// The length of the table in bytes, including the header, but excluding the strings.
    uint8_t length;
    /// Unique handle for this table, used to reference it in other tables.
    uint16_t handle;

    /// Calculate the full length of the table, including the strings.
    [[nodiscard]] size_t calculateFullLength() const;

    /// Calculate the number of strings in the table.
    [[nodiscard]] size_t calculateStringCount() const;

    /// Get the string at the given index from the string table.
    [[nodiscard]] const char* getString(size_t number) const;
} __attribute__ ((packed));

/// Provides convenient access to the SMBIOS tables from a reference to the first table.
/// This class implements the `Iterable` interface, allowing iteration over the tables,
/// as well as the `[]` operator to access tables by their type.
/// It is intended to be used in kernel space, where the SMBIOS tables are found in memory during the boot process.
/// For user space, the `/device/smbios/tables` directory is provided, which contains the tables as files.
///
/// ### Example
/// ```c++
///     // `firstTable` is a pointer to the first SMBIOS table in memory.
///     const auto *tables = new Util::Hardware::SmBios::Tables(*firstTable);
///
///     // Iterate over all tables and print their types.
///     for (const auto &table : *tables) {
///         Util::System::out << "Found SMBIOS table with type: "
///             << static_cast<uint32_t>(table.header.type) << Util::Io::PrintStream::endl;
///     }
///
///     Util::System::out << Util::Io::PrintStream::flush;
///
///     // Delete `tables` (does not affect `firstTable).
///     delete tables;
/// ```
class Tables final : public Iterable<const TableHeader> {

public:
    /// Create an empty `Tables` instance.
    Tables() = default;

    /// Create a `Tables` instance from a reference to the first table.
    explicit Tables(const TableHeader &firstTable);

    /// Tables is not copyable, since all copies would reference the same memory.
    Tables(const Tables &other) = delete;

    /// Tables is not assignable, since all copies would reference the same memory.
    Tables& operator=(const Tables &other) = delete;

    /// Tables only contains a pointer to the first table, so the default destructor is sufficient.
    ~Tables() override = default;

    /// Access a table by its type via the `[]` operator.
    /// If no table with the given type exists, a panic is fired.
    [[nodiscard]] const TableHeader& operator[](HeaderType type) const;

    /// Get the number of tables.
    [[nodiscard]] size_t getTableCount() const;

    /// Check if a table with the given type exists.
    [[nodiscard]] bool hasTable(HeaderType type) const;

    /// Get an array of all available table types.
    [[nodiscard]] Array<HeaderType> getTypes() const;

    /// Get an iterator to the first table.
    /// This allows iteration over the tables using a range-based for loop.
    [[nodiscard]] Iterator<const TableHeader> begin() const override;

    /// Get an iterator to the end of the tables.
    /// This allows iteration over the tables using a range-based for loop.
    /// The end iterator actually points to NULL, indicating the end of the iteration.
    [[nodiscard]] Iterator<const TableHeader> end() const override;

    /// Get the next table in the iteration based on the current table.
    [[nodiscard]] IteratorElement<const TableHeader>
        next(const IteratorElement<const TableHeader> &element) const override;

private:

    const TableHeader *firstTable = nullptr;
};

/// BIOS characteristics flags used in the `BiosInformation` table.
enum BiosCharacteristic : uint64_t {
    CHARACTERISTICS_NOT_SUPPORTED = 0x0000000000000004,
    ISA_SUPPORTED = 0x0000000000000008,
    MCA_SUPPORTED = 0x0000000000000010,
    EISA_SUPPORTED = 0x0000000000000020,
    PCI_SUPPORTED = 0x0000000000000040,
    PCMCIA_SUPPORTED = 0x0000000000000080,
    PLUG_AND_PLAY_SUPPORTED = 0x0000000000000100,
    APM_SUPPORTED = 0x0000000000000200,
    FIRMWARE_UPGRADEABLE = 0x0000000000000400,
    FIRMWARE_SHADOWING_ALLOWED = 0x0000000000000800,
    VL_VESA_SUPPORTED = 0x0000000000001000,
    ESCD_SUPPORTED = 0x0000000000002000,
    CD_BOOT_SUPPORTED = 0x0000000000004000,
    SELECTABLE_BOOT_SUPPORTED = 0x0000000000008000,
    FIRMWARE_ROM_SOCKETED = 0x0000000000010000,
    PCMCIA_BOOT_SUPPORTED = 0x0000000000020000,
    EDD_SUPPORTED = 0x0000000000040000,
    INT13_NEC9800_FLOPPY_SUPPORTED = 0x0000000000080000,
    INT13_TOSHIBA_FLOPPY_SUPPORTED = 0x0000000000100000,
    INT13_360K_FLOPPY_SUPPORTED = 0x0000000000200000,
    INT13_1_2M_FLOPPY_SUPPORTED = 0x0000000000400000,
    INT13_720K_FLOPPY_SUPPORTED = 0x0000000000800000,
    INT13_2_88M_FLOPPY_SUPPORTED = 0x0000000001000000,
    INT5_PRINT_SCREEN_SERVICE_SUPPORTED = 0x0000000002000000,
    INT9_KEYBOARD_SERVICE_SUPPORTED = 0x0000000004000000,
    INT14_SERIAL_SERVICE_SUPPORTED = 0x0000000080000000,
    INT17_PRINTER_SERVICE_SUPPORTED = 0x0000000010000000,
    INT10_VIDEO_SERVICE_SUPPORTED = 0x0000000020000000,
    NEC_PC98 = 0x0000000040000000,
};

/// First BIOS characteristics flags extension used in the `BiosInformation` table.
enum BiosCharacteristicExtension1 : uint8_t {
    ACPI_SUPPORTED = 0x01,
    USB_LEGACY_SUPPORTED = 0x02,
    AGP_SUPPORTED = 0x04,
    I2O_BOOT_SUPPORTED = 0x08,
    LS120_BOOT_SUPPORTED = 0x10,
    ATAPI_ZIP_DRIVE_BOOT_SUPPORTED = 0x20,
    FIREWIRE_BOOT_SUPPORTED = 0x40,
    SMART_BATTERY_SUPPORTED = 0x80
};

/// Second BIOS characteristics flags extension used in the `BiosInformation` table.
enum BiosCharacteristicExtension2 : uint8_t {
    BIOS_BOOT_SPECIFICATION_SUPPORTED = 0x01,
    FUNCTION_KEY_INITIATED_NETWORK_BOOT_SUPPORTED = 0x02,
    TARGETED_CONTENT_DISTRIBUTION_ENABLED = 0x04,
    UEFI_SUPPORTED = 0x08,
    VIRTUAL_MACHINE = 0x10,
    MANUFACTURING_MODE_SUPPORTED = 0x20,
    MANUFACTURING_MODE_ENABLED = 0x40
};

/// The BIOS Information table provides information about the BIOS firmware.
struct BiosInformation {
    /// The header that prepends every SMBIOS table.
    TableHeader header;
    /// Name of the BIOS firmware vendor (as an index into the string table).
    uint8_t vendorString;
    /// Version of the BIOS firmware (as an index into the string table).
    uint8_t versionString;
    /// Segment where the BIOS ROM starts in memory. On UEFI systems, this is usually 0x0000.
    uint16_t startAddressSegment;
    /// BIOS firmware release date (as an index into the string table).
    uint8_t releaseDateString;
    /// BIOS ROM size multiplicator (ROM size = (`romSize` + 1) * 64 KiB).
    uint8_t romSize;
    /// BIOS characteristics flags, indicating various capabilities, as described by the `BiosCharacteristic` enum.
    uint64_t characteristics;
    /// First extension of the BIOS characteristics flags, as described by the `BiosCharacteristicExtension1` enum.
    uint8_t characteristicsExtension1;
    /// Second extension of the BIOS characteristics flags, as described by the `BiosCharacteristicExtension2` enum.
    uint8_t characteristicsExtension2;
    /// BIOS firmware version (major number).
    uint8_t majorVersion;
    /// BIOS firmware version (minor number).
    uint8_t minorVersion;
    /// Embedded controller firmware version (major number).
    uint8_t embeddedControllerMajorVersion;
    /// Embedded controller firmware version (minor number).
    uint8_t embeddedControllerMinorVersion;
    /// Extended size of the physical device containing the BIOS firmware.
    /// Bits [13:0]: Firmware size
    /// Bits [15:14]: Unit [00] = MiB, [01] = GiB
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
