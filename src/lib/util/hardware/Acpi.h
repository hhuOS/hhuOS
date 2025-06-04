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

#ifndef HHUOS_LIB_UTIL_HARDWARE_ACPI_H
#define HHUOS_LIB_UTIL_HARDWARE_ACPI_H

#include <stdint.h>
#include <stddef.h>

#include "base/String.h"
#include "collection/Array.h"
#include "collection/Iterator.h"

/// Structures and definitions for the Advanced Configuration and Power Interface (ACPI)
/// In user space, the ACPI structures are accessible via the '/device/acpi' directory.
namespace Util::Hardware::Acpi {

/// The root system description pointer (`Rsdp`) is the entry point for the ACPI tables.
/// On boot, the operating system searches for the RSDP in memory or gets it from the bootloader.
/// It contains the address of the root system description table (`Rsdt`),
/// which in turn contains the addresses of other ACPI tables.
/// In user space, the `Rsdp` can be found at '/device/acpi/rsdp'.
struct Rsdp {
    /// Signature of the RSDP, which is always "RSD PTR " (8 bytes).
    char signature[8];
    /// A checksum of the RSDP structure, used to verify its integrity.
    /// To verify the checksum, sum up the structure byte by byte and check if the lower 8 bits of the sum are zero.
    /// This is already implemented in the `verifyChecksum()` method.
    uint8_t checksum;
    /// A 6-character string identifying the OEM.
    char oemId[6];
    /// ACPI revision number, either 0 for ACPI 1.0 or 2 for ACPI 2.0 or later.
    uint8_t revision;
    /// The address of the root system description table (`Rsdt`).
    uint32_t rsdtAddress;

    /// Verify the checksum of this RSDP structure.
    [[nodiscard]] bool verifyChecksum() const;
} __attribute__ ((packed));

/// The system description table (SDT) header is the common header for all ACPI tables.
/// It contains metadata about the table, such as its signature, length, revision, and checksum.
struct SdtHeader {
    /// Each ACPI table has a unique signature, which is a 4-character ASCII string
    /// (e.g. "RSDT" for the Root System Description Table or "FACP" for the Fixed ACPI Description Table).
    char signature[4];
    /// The length of the table in bytes, including the header.
    uint32_t length;
    /// Revision number of the table. Newer revisions are backwards compatible to older ones.
    uint8_t revision;
    /// A checksum of the entire table, used to verify its integrity.
    uint8_t checksum;
    /// A 6-character string identifying the OEM.
    char oemId[6];
    /// An 8-character string used by the OEM to identify the table.
    char oemTableId[8];
    /// An revision number given by the OEM.
    uint32_t oemRevision;
    /// Vendor ID describing the utility that created the table.
    uint32_t creatorId;
    /// Revision number of the utility that created the table.
    uint32_t creatorRevision;

    /// Verify the checksum of this table.
    [[nodiscard]] bool verifyChecksum() const;
} __attribute__ ((packed));

/// The root system description table (`Rsdt`) is the first ACPI table and is pointed to by the `Rsdp`.
/// Its signature is "RSDT" and it contains an array of pointers to other ACPI tables.
/// The amount of pointers is variable and can be calculated from the length of the table.
/// The addresses of the tables are physical addresses, and need to be mapped into virtual memory
/// before they can be accessed. To simplify access to the `Rsdt`, the class `Tables` is provided,
/// which maps all tables into virtual memory on construction and allows iteration over them.
struct Rsdt {
    /// The header that prepends every ACPI table.
    SdtHeader header;
    /// An array of pointers (physical addresses) to other ACPI tables.
    /// The number of pointers can be calculated from the length of the table:
    /// ```
    /// header.length - sizeof(SdtHeader) / sizeof(SdtHeader*).
    /// ```
    /// This is already implemented in the `getTableCount()` method.
    SdtHeader* tables[];

    /// Get the number of tables in this `Rsdt`.
    [[nodiscard]] size_t getTableCount() const;
} __attribute__ ((packed));

/// Provides convenient access to the ACPI tables listed in the `Rsdt`.
/// Using the `Rsdt` structure directly is not recommended, as it only contains physical addresses of the tables,
/// which are not accessible after paging is enabled.
/// This class maps the tables into virtual memory (on the heap) on construction and frees them on destruction.
/// Furthermore, it implements the `begin()` and `end()` methods to allow iteration over the tables,
/// as well as the `[]` operator to access tables by index or signature.
///
/// ### Example
/// ```c++
///     // `rsdt` is a pointer to a `Util::Hardware::Acpi::Rsdt` structure.
///     const auto *tables = new Util::Hardware::Acpi::MappedRsdt(*rsdt);
///
///     // Iterate over all tables and print their signatures.
///     for (const auto &table : *tables) {
///         printf("Found ACPI table: %s\n", table.signature);
///     }
///
///     // Delete `tables`, unmapping all tables from virtual memory.
///     delete tables;
/// ```
class Tables final : public Iterable<SdtHeader> {

public:
    /// Construct a `MappedRsdt` from a `Rsdt`.
    /// This will map all tables into virtual memory (on the heap) and store the pointers in the `tables` array.
    explicit Tables(const Rsdt &rsdt);

    /// MappedRsdt is not copyable, since all copies would reference the same mapped memory,
    /// so the copy constructor is deleted.
    Tables(const Tables &rsdt) = delete;

    /// MappedRsdt is not assignable, since all copies would reference the same mapped memory,
    /// so the assignment operator is deleted.
    Tables& operator=(const Tables &rsdt) = delete;

    /// Delete the structure and unmap all tables from virtual memory.
    ~Tables() override;

    /// Access a table by its index via the `[]` operator.
    /// If the index is out of bounds, a panic is fired by the underlying `Array` class.
    const SdtHeader& operator[](size_t index) const;

    /// Access a table by its signature via the `[]` operator.
    /// If the signature is not found, a panic is fired. Use `hasTable()` to check if a table exists.
    const SdtHeader& operator[](const char *signature) const;

    /// Access a table by its signature via the `[]` operator.
    /// If the signature is not found, a panic is fired. Use `hasTable()` to check if a table exists.
    const SdtHeader& operator[](const String &signature) const;

    /// Get the number of tables.
    [[nodiscard]] size_t getTableCount() const;

    /// Check if a table with the given signature exists.
    [[nodiscard]] bool hasTable(const char *signature) const;

    /// Get an array of all table signatures.
    [[nodiscard]] Array<String> getSignatures() const;

    /// Get an iterator to the beginning of the table pointers.
    /// This allows iteration over the tables using a range-based for loop.
    [[nodiscard]] Iterator<SdtHeader> begin() const override;

    /// Get an iterator to the end of the table pointers.
    /// This allows iteration over the tables using a range-based for loop.
    [[nodiscard]] Iterator<SdtHeader> end() const override;

    /// Get the next table in the iteration based on the current table.
    [[nodiscard]] IteratorElement<SdtHeader> next(const IteratorElement<SdtHeader> &element) const override;

private:

    Array<SdtHeader*> tables;
};

/// Enumeration of power management profiles used in the `Fadt`.
enum PowerManagementProfile : uint8_t {
    UNSPECIFIED = 0x00,
    DESKTOP = 0x01,
    MOBILE = 0x02,
    WORKSTATION = 0x03,
    ENTERPRISE_SERVER = 0x04,
    SOHO_SERVER = 0x05,
    APPLIANCE_PC = 0x06,
    PERFORMANCE_SERVER = 0x07
};

/// Enumeration of boot architecture flags used in the `Fadt`.
enum BootArchitectureFlag : uint8_t {
    LEGACY_DEVICE = 0x01,
    PS2_KEYBOARD_CONTROLLER = 0x02,
    VGA_NOT_PRESENT = 0x04,
    MSI_NOT_SUPPORTED = 0x08,
    PCIE_ASPM_CONTROLS = 0x10,
    CMOS_RTC_NOT_PRESENT = 0x20
};

/// Enumeration of address spaces used in the `GenericAddressStructure`.
enum AddressSpace : uint8_t {
    SYSTEM_MEMORY = 0x00,
    SYSTEM_IO = 0x01,
    PCI_CONFIGURATION_SPACE = 0x02,
    EMBEDDED_CONTROLLER = 0x03,
    SYSTEM_MANAGEMENT_BUS = 0x04,
    SYSTEM_CMOS = 0x05,
    PCI_DEVICE_BAR_TARGET = 0x06,
    IPMI = 0x07,
    GENERAL_PURPOSE_IO = 0x08,
    GENERIC_SERIAL_BUS = 0x09,
    PLATFORM_COMMUNICATION_CHANNEL = 0x0a
};

/// Enumeration of access sizes used in the `GenericAddressStructure`.
enum AccessSize : uint8_t {
    UNDEFINED = 0x00,
    BYTE = 0x01, // 8 bits
    WORD = 0x02, // 16 bits
    DWORD = 0x03, // 32 bits
    QWORD = 0x04 // 64 bits
};

/// The generic address structure is used to describe memory regions in ACPI tables.
/// For example, a device such as the HPET (High Precision Event Timer) uses this structure
/// to describe its memory-mapped I/O region.
struct GenericAddressStructure {
    /// The type of address space, where the structure/register exists (e.g. system memory (MMIO), I/O space, etc.).
    AddressSpace addressSpace;
    /// When addressing a register, this field indicates the width of the register in bits.
    /// When addressing a structure, this field must be set to 0.
    uint8_t bitWidth;
    /// The offset of the register at the given `address`.
    /// When addressing a structure, this field must be set to 0.
    uint8_t bitOffset;
    /// The access size of the register, indicating how many bytes must be read/written at once.
    AccessSize accessSize;
    /// The address where the register or structure is located at in the given address space.
    uint64_t address;
} __attribute__ ((packed));

/// The fixed ACPI description table (`Fadt`) provides power management and system configuration information.
/// Its signature is "FACP".
struct Fadt {
    /// The header that prepends every ACPI table.
    SdtHeader header;
    /// Physical address of the Firmware Control ACPI Structure ("FACS").
    uint32_t firmwareControl;
    /// Physical address of the Differentiated System Description Table ("DSDT").
    uint32_t dsdtAddress;
    /// Was used in ACPI 1.0, but deprecated in ACPI 2.0 and later and is now reserved.
    uint8_t reserved1;
    /// The preferred power management profile for the system, set by the OEM.
    /// For example, a desktop system would use `DESKTOP`, while a laptop would use `MOBILE`.
    PowerManagementProfile preferredPowerManagementProfile;
    /// Interrupt vector for the System Control Interrupt (SCI).
    uint16_t sciInterrupt;
    /// Address of the SMI command port, which for example allows the OS to take control over the ACPI registers.
    uint32_t smiCommandPort;
    /// Value to write to the SMI command port to disable SMI ownership of the ACPI hardware registers.
    uint8_t acpiEnable;
    /// Value to write to the SMI command port to re-enable SMI ownership of the ACPI hardware registers.
    uint8_t acpiDisable;
    /// Value to write to the SMI command port to enter the S4BIOS state (0 = S4BIOS not supported).
    uint8_t s4BiosRequest;
    /// Value to write to the SMI command port to take responsibility over the CPU power state (0 = not supported).
    uint8_t pStateControl;
    /// Address of the PM1a event block (required field).
    uint32_t pm1aEventBlock;
    /// Address of the PM1b event block (0 = not present).
    uint32_t pm1bEventBlock;
    /// Address of the PM1a control register block (required field).
    uint32_t pm1aControlBlock;
    /// Address of the PM1b control register block (0 = not present).
    uint32_t pm1bControlBlock;
    /// Address of the PM2 control register block (0 = not present).
    uint32_t pm2ControlBlock;
    /// Address of the power management timer block (0 = not present).
    uint32_t pmTimerBlock;
    /// Address of the General Purpose Event 0 block (0 = not present).
    uint32_t gpe0Block;
    /// Address of the General Purpose Event 1 block (0 = not present).
    uint32_t gpe1Block;
    /// Number of bytes decoded by the PM1a event block and PM1b event block if supported (always >= 4).
    uint8_t pm1EventLength;
    /// Number of bytes decoded by the PM1a control block and PM1b control block if supported (always >= 2).
    uint8_t pm1ControlLength;
    /// Number of bytes decoded by the PM2 control block (0 = not present, else >= 1).
    uint8_t pm2ControlLength;
    /// Number of bytes decoded by the power management timer block (0 = not present, else = 4).
    uint8_t pmTimerLength;
    /// Length of the register pointed to by the GPE0 block.
    uint8_t gpe0Length;
    /// Length of the register pointed to by the GPE1 block.
    uint8_t gpe1Length;
    /// Base address of GPE1 based events.
    uint8_t gpe1Base;
    /// Value to write to the SMI command port to indicate OS support for C states (0 = not supported).
    uint8_t cStateControl;
    /// Worst-case latency in microseconds for to enter/exit a C2 state (>100 means not supported).
    uint16_t worstC2Latency;
    /// Worst-case latency in microseconds for to enter/exit a C3 state (>1000 means not supported).
    uint16_t worstC3Latency;
    /// Number of flush strides that need to be read to completely flush the processor's cache.
    uint16_t flushSize;
    /// Cache line width in bytes of the processor's cache.
    uint16_t flushStride;
    /// Index of the processor's duty cycle setting within the according register.
    uint8_t dutyOffset;
    /// Bit width of the processor's duty cycle setting value.
    uint8_t dutyWidth;
    /// Address of the real time clock's day of month alarm register in CMOS memory (0 = not present).
    uint8_t dayAlarm;
    /// Address of the real time clock's month of year alarm register in CMOS memory (0 = not present).
    uint8_t monthAlarm;
    /// Address of the real time clock's century register in CMOS memory (0 = not present).
    uint8_t century;
    uint16_t bootArchitectureFlags;
    uint8_t reserved2;
    uint32_t flags;
} __attribute__ ((packed));

/// Enumeration of different structure types listed in the Multiple APIC Description Table (`Madt`).
enum ApicStructureType : uint8_t {
    PROCESSOR_LOCAL_APIC = 0x0,
    IO_APIC = 0x1,
    INTERRUPT_SOURCE_OVERRIDE = 0x2,
    NON_MASKABLE_INTERRUPT_SOURCE = 0x3,
    LOCAL_APIC_NMI = 0x4,
};

/// Enumeration of flags describing the behavior of an interrupt source in the
/// Multiple APIC Description Table (`Madt`).
enum IntiFlag : uint8_t {
    ACTIVE_HIGH = 0x1,
    ACTIVE_LOW = 0x3,
    EDGE_TRIGGERED = 0x4,
    LEVEL_TRIGGERED = 0xc
};

/// Common header for all structures in the Multiple APIC Description Table (`Madt`).
struct ApicStructureHeader {
    ApicStructureType type;
    uint8_t length;
} __attribute__ ((packed));

/// Information about a local APIC (Advanced Programmable Interrupt Controller).
/// Typically, there is one local APIC per CPU core. Local APICs can issue inter-processor interrupts (IPIs)
/// and handle local interrupts (e.g. from the APIC timer). External interrupts (e.g. from I/O devices)
/// are handled by the I/O APIC, which routes them to the appropriate local APICs.
struct ProcessorLocalApic {
    ApicStructureHeader header;
    uint8_t acpiProcessorId;
    uint8_t apicId;
    uint32_t flags; // "Enabled" flag
} __attribute__ ((packed));

/// Information about an I/O APIC (Input/Output Advanced Programmable Interrupt Controller).
/// Typically, there is one I/O APIC per system, although some systems may have multiple I/O APICs.
/// The I/O APIC receives interrupts from I/O devices and routes them to the appropriate local APICs.
struct IoApic {
    ApicStructureHeader header;
    uint8_t ioApicId;
    uint8_t reserved;
    uint32_t ioApicAddress;
    uint32_t globalSystemInterruptBase;
} __attribute__ ((packed));

/// Describes an override for an interrupt source.
/// Such overrides are used to remap IRQs to global system interrupts (GSIs).
/// For example, an override can be used to map an IRQ 0 to GSI 2.
/// All overrides must be parsed by the system and applied to the I/O APIC.
struct InterruptSourceOverride {
    ApicStructureHeader header;
    uint8_t bus; // "0 - Constant, meaning ISA"
    uint8_t source; // ISA IRQ relative if bus = 0
    uint32_t globalSystemInterrupt;
    uint16_t flags; // INTI Flags
} __attribute__ ((packed));

/// Describes a non-maskable interrupt (NMI) source.
/// These interrupts are typically used for critical events that cannot be masked or ignored.
/// All NMI sources must be parsed by the system and applied to the I/O APIC.
struct NmiSource {
    ApicStructureHeader header;
    uint16_t flags; // INTI Flags
    uint32_t globalSystemInterrupt;
} __attribute__ ((packed));

/// Describes a non-maskable interrupt (NMI) source that is associated with a local APIC.
struct LocalApicNmi {
    ApicStructureHeader header;
    uint8_t acpiProcessorId;
    uint16_t flags; // INTI Flags
    uint8_t localApicLint;
} __attribute__ ((packed));

/// The Multiple APIC Description Table (`Madt`) provides information about all
/// Advanced Programmable Interrupt Controllers (APICs) in the system.
/// Typically, this includes local APICs for each CPU core and one or multiple I/O APICs.
/// Furthermore, it contains information about interrupt source overrides and non-maskable interrupt sources.
/// All APIC structures in the `Madt` are listed directly after each other in memory
/// and are accessible via the `apicStructure` field. By using the `length` field of each structure,
/// one can determine the size of each structure and iterate through the list.
struct Madt {
    SdtHeader header;
    uint32_t localApicAddress;
    uint32_t flags; // PCAT compat flag
    ApicStructureHeader apicStructure; // Is a list
} __attribute__ ((packed));

/// This table provides information about the High Precision Event Timer (HPET).
struct Hpet {
    SdtHeader header;
    uint8_t hardwareRevision;
    uint8_t comparatorCount: 5;
    uint8_t counterSize: 1;
    uint8_t reserved: 1;
    uint8_t legacy_replacement: 1;
    uint16_t pciVendorId;
    GenericAddressStructure address;
    uint8_t hpetNumber;
    uint16_t minimumTick;
    uint8_t pageProtection;
} __attribute__((packed));

/// The Boot Graphics Resource Table (`Bgrt`) provides information about the vendor-specific boot graphics image,
/// shown during BIOS initialization. It can be used to display the manufacturer's logo during the boot process,
/// providing a seemingly seamless transition from the BIOS to the operating system.
/// The data pointed to by `imageAddress` is not raw pixel data, but rather a bitmap file. Information such as
/// the image size and bit depth can be found in the bitmap header (pointed to by `imageAddress`).
struct Bgrt {
    SdtHeader header;
    uint16_t version;
    uint8_t status;
    uint8_t imageType;
    uint64_t imageAddress;
    uint32_t imageOffsetX;
    uint32_t imageOffsetY;
} __attribute__ ((packed));

};

#endif
