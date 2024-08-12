/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_UTIL_ACPI_H
#define HHUOS_UTIL_ACPI_H

#include <stdint.h>

namespace Util::Hardware {

class Acpi {

public:

    struct Rsdp {
        char signature[8];
        uint8_t checksum;
        char oemId[6];
        uint8_t revision;
        uint32_t rsdtAddress;
    } __attribute__ ((packed));

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

    struct Rsdt {
        SdtHeader header;
        const SdtHeader* tables[];
    } __attribute__ ((packed));

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

    enum BootArchitectureFlag : uint8_t {
        LEGACY_DEVICE = 0x01,
        PS2_KEYBOARD_CONTROLLER = 0x02,
        VGA_NOT_PRESENT = 0x04,
        MSI_NOT_SUPPORTED = 0x08,
        PCIE_ASPM_CONTROLS = 0x10,
        CMOS_RTC_NOT_PRESENT = 0x20
    };

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

    struct GenericAddressStructure {
        AddressSpace addressSpace;
        uint8_t bitWidth;
        uint8_t bitOffset;
        uint8_t accessSize;
        uint64_t address;
    } __attribute__ ((packed));

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

    // Only ACPI 1.0, later versions have more stuff that needs to be accounted for
    enum ApicStructureType : uint8_t {
        PROCESSOR_LOCAL_APIC = 0x0,
        IO_APIC = 0x1,
        INTERRUPT_SOURCE_OVERRIDE = 0x2,
        NON_MASKABLE_INTERRUPT_SOURCE = 0x3,
        LOCAL_APIC_NMI = 0x4,
    };

    enum IntiFlag : uint8_t {
        ACTIVE_HIGH = 0x1,
        ACTIVE_LOW = 0x3,
        EDGE_TRIGGERED = 0x4,
        LEVEL_TRIGGERED = 0xc
    };

    struct ApicStructureHeader {
        ApicStructureType type;
        uint8_t length;
    } __attribute__ ((packed));

    struct ProcessorLocalApic {
        ApicStructureHeader header;
        uint8_t acpiProcessorId;
        uint8_t apicId;
        uint32_t flags; // "Enabled" flag
    } __attribute__ ((packed));

    struct IoApic {
        ApicStructureHeader header;
        uint8_t ioApicId;
        uint8_t reserved;
        uint32_t ioApicAddress;
        uint32_t globalSystemInterruptBase;
    } __attribute__ ((packed));

    struct InterruptSourceOverride {
        ApicStructureHeader header;
        uint8_t bus; // "0 - Constant, meaning ISA"
        uint8_t source; // ISA IRQ relative if bus = 0
        uint32_t globalSystemInterrupt;
        uint16_t flags; // INTI Flags
    } __attribute__ ((packed));

    struct NmiSource {
        ApicStructureHeader header;
        uint16_t flags; // INTI Flags
        uint32_t globalSystemInterrupt;
    } __attribute__ ((packed));

    struct LocalApicNmi {
        ApicStructureHeader header;
        uint8_t acpiProcessorId;
        uint16_t flags; // INTI Flags
        uint8_t localApicLint;
    } __attribute__ ((packed));

    struct Madt {
        SdtHeader header;
        uint32_t localApicAddress;
        uint32_t flags; // PCAT compat flag
        ApicStructureHeader apicStructure; // Is a list
    } __attribute__ ((packed));

    struct Hpet : public SdtHeader {
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

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Acpi() = default;

    /**
     * Copy Constructor.
     */
    Acpi(const Acpi &other) = delete;

    /**
     * Assignment operator.
     */
    Acpi &operator=(const Acpi &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Acpi() = delete;
};

}

#endif
