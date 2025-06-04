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

/// Structures and definitions for the Advanced Configuration and Power Interface (ACPI)
/// In user space, the ACPI structures are accessible via the '/device/acpi' directory.
namespace Util::Hardware::Acpi {

/// The root system description pointer (`Rsdp`) is the entry point for the ACPI tables.
/// On boot, the operating system searches for the RSDP in memory or gets it from the bootloader.
/// It contains the address of the root system description table (`Rsdt`),
/// which in turn contains the addresses of other ACPI tables.
/// In user space, the `Rsdp` can be found at '/device/acpi/rsdp'.
struct Rsdp {
    char signature[8];
    uint8_t checksum;
    char oemId[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} __attribute__ ((packed));

/// The system description table (SDT) header is the common header for all ACPI tables.
/// It contains metadata about the table, such as its signature, length, revision, and checksum.
struct SdtHeader {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemId[6];
    char oemTableId[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
} __attribute__ ((packed));

/// The root system description table (`Rsdt`) is the first ACPI table and is pointed to by the `Rsdp`.
/// It contains an array of pointers to other ACPI tables.
/// The amount of pointers is variable and can be calculated from the length of the table.
/// Its signature is "RSDT".
struct Rsdt {
    SdtHeader header;
    const SdtHeader* tables[];
} __attribute__ ((packed));

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

/// The generic address structure is used to describe memory regions in ACPI tables.
/// For example, a device such as the HPET (High Precision Event Timer) uses this structure
/// to describe its memory-mapped I/O region.
struct GenericAddressStructure {
    AddressSpace addressSpace;
    uint8_t bitWidth;
    uint8_t bitOffset;
    uint8_t accessSize;
    uint64_t address;
} __attribute__ ((packed));

/// The fixed ACPI description table (`Fadt`) provides power management and system configuration information.
struct Fadt {
    SdtHeader header;
    uint32_t firmwareControl;
    uint32_t dsdtAddress;
    uint8_t reserved1;
    PowerManagementProfile preferredPowerManagementProfile;
    uint16_t sciInterrupt;
    uint32_t smiCommandPort;
    uint8_t acpiEnable;
    uint8_t acpiDisable;
    uint8_t s4BiosRequest;
    uint8_t pStateControl;
    uint32_t pm1aEventBlock;
    uint32_t pm1bEventBlock;
    uint32_t pm1aControlBlock;
    uint32_t pm1bControlBlock;
    uint32_t pm2ControlBlock;
    uint32_t pmTimerBlock;
    uint32_t gpe0Block;
    uint32_t gpe1Block;
    uint8_t pm1EventLength;
    uint8_t pm1ControlLength;
    uint8_t pm2ControlLength;
    uint8_t pmTimerLength;
    uint8_t gpe0Length;
    uint8_t gpe1Length;
    uint8_t gpe1Base;
    uint8_t cStateControl;
    uint16_t worstC2Latency;
    uint16_t worstC3Latency;
    uint16_t flushSize;
    uint16_t flushOverride;
    uint8_t dutyOffset;
    uint8_t dutyWidth;
    uint8_t dayAlarm;
    uint8_t monthAlarm;
    uint8_t century;
    uint16_t bootArchitectureFlags;
    uint8_t reserved2;
    uint32_t flags;
} __attribute__ ((packed));

/// Enumeration of different structure type listed in the Multiple APIC Description Table (`Madt`).
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
