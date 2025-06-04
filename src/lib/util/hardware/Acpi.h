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
#include "collection/ArrayList.h"
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
/// This class is mainly intended to be used in kernel space, to initialize the ACPI subsystem and access the tables.
/// For user space, the `/device/acpi/tables` directory is provided, which contains the tables as files.
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
    /// Create an empty `Tables` instance.
    Tables() = default;

    /// Create a `Tables` instance from a `Rsdt`.
    /// This will map all tables into virtual memory (on the heap) and store the pointers in the `tables` array.
    explicit Tables(const Rsdt &rsdt);

    /// Tables is not copyable, since all copies would reference the same mapped memory,
    /// so the copy constructor is deleted.
    Tables(const Tables &other) = delete;

    /// Tables is not assignable, since all copies would reference the same mapped memory,
    /// so the assignment operator is deleted.
    Tables& operator=(const Tables &other) = delete;

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

    Array<SdtHeader*> tables = Array<SdtHeader*>(0);
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
enum BootArchitectureFlag : uint16_t {
    LEGACY_DEVICE = 0x01,
    PS2_KEYBOARD_CONTROLLER = 0x02,
    VGA_NOT_PRESENT = 0x04,
    MSI_NOT_SUPPORTED = 0x08,
    PCIE_ASPM_CONTROLS = 0x10,
    CMOS_RTC_NOT_PRESENT = 0x20
};

/// Enumeration of flags used in the `Fadt`.
enum FadtFlag : uint32_t {
    WBINVD = 0x00000001,
    WBINVD_FLUSH = 0x00000002,
    PROC_C1 = 0x00000004,
    P_LVL2_UP = 0x00000008,
    POWER_BUTTON = 0x00000010,
    SLP_BUTTON = 0x00000020,
    FIXED_RTC = 0x00000040,
    RTC_S4 = 0x00000080,
    EXTENDED_TIMER_VALUE = 0x00000100,
    DOCKING_CAPABILITY = 0x00000200,
    RESET_REGISTER = 0x00000400,
    SEALED_CASE = 0x00000800,
    HEADLESS = 0x00001000,
    CPU_SW_SLEEP = 0x00002000,
    PCI_EXPRESS_WAKEUP = 0x00004000,
    USE_PLATFORM_CLOCK = 0x00008000,
    S4_RTC_STS_VALID = 0x00010000,
    REMOTE_POWER_ON_CAPABILITY = 0x00020000,
    FORCE_APIC_CLUSTER_MODEL = 0x00040000,
    FORCE_APIC_PHYSICAL_DESTINATION_MODE = 0x00080000,
    HARDWARE_REDUCED_ACPI = 0x00100000,
    LOW_POWER_S0_IDLE_CAPABLE = 0x00200000,
    PERSISTENT_CPU_CACHES = 0x00400000,
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
    /// IA-PC boot architecture flags, as described by the `BootArchitectureFlag` enum.
    uint16_t bootArchitectureFlags;
    /// Must be 0.
    uint8_t reserved2;
    /// ACPI flags, as described by the `FadtFlag` enum.
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

/// Enumeration of flags used in the `ProcessorLocalApic` structure.
enum LocalApicFlag : uint32_t {
    ENABLED = 0x1,
    ONLINE_CAPABLE = 0x2,
};

/// Enumeration of flags describing the behavior of an interrupt source in the
/// Multiple APIC Description Table (`Madt`).
enum IntiFlag : uint8_t {
    ACTIVE_HIGH = 0x1,
    ACTIVE_LOW = 0x3,
    EDGE_TRIGGERED = 0x4,
    LEVEL_TRIGGERED = 0xc
};

/// Enumeration of interrupt source buses used in the `InterruptSourceOverride` structure.
enum InterruptBus : uint8_t {
    ISA = 0x00
};

/// Enumeration of flags used in the `Madt`.
enum MadtFlag : uint32_t {
    PCAT_COMPATIBLE = 0x01
};

/// Common header for all structures in the Multiple APIC Description Table (`Madt`).
struct ApicStructureHeader {
    /// Determines the type of the structure, as described by the `ApicStructureType` enum.
    ApicStructureType type;
    /// The length of the structure in bytes, including this header.
    uint8_t length;
} __attribute__ ((packed));

/// Information about a local APIC (Advanced Programmable Interrupt Controller).
/// Typically, there is one local APIC per CPU core. Local APICs can issue inter-processor interrupts (IPIs)
/// and handle local interrupts (e.g. from the APIC timer). External interrupts (e.g. from I/O devices)
/// are handled by the I/O APIC, which routes them to the appropriate local APICs.
struct ProcessorLocalApic {
    /// The header that prepends every APIC structure.
    ApicStructureHeader header;
    /// The ACPI processor ID used to identify the CPU (Deprecated).
    uint8_t acpiProcessorId;
    /// The processor's local APIC ID, which is unique for each local APIC (and thus CPU core) in the system.
    uint8_t apicId;
    /// Flags describing the behavior of the local APIC, as described by the `LocalApicFlag` enum.
    uint32_t flags;
} __attribute__ ((packed));

/// Information about an I/O APIC (Input/Output Advanced Programmable Interrupt Controller).
/// Typically, there is one I/O APIC per system, although some systems may have multiple I/O APICs.
/// The I/O APIC receives interrupts from I/O devices and routes them to the appropriate local APICs.
struct IoApic {
    /// The header that prepends every APIC structure.
    ApicStructureHeader header;
    /// The I/O APIC's unique ID.
    uint8_t ioApicId;
    /// Must be 0.
    uint8_t reserved;
    /// The memory address of the I/O APIC's registers, accessible via MMIO (Memory-Mapped I/O).
    uint32_t ioApicAddress;
    /// The global system interrupt base, which is the first GSI that the I/O APIC can handle.
    uint32_t globalSystemInterruptBase;
} __attribute__ ((packed));

/// Describes an override for an interrupt source.
/// Such overrides are used to remap IRQs to global system interrupts (GSIs).
/// For example, an override can be used to map an IRQ 0 to GSI 2.
/// All overrides must be parsed by the system and applied to the I/O APIC.
struct InterruptSourceOverride {
    /// The header that prepends every APIC structure.
    ApicStructureHeader header;
    /// The bus on which the interrupt source is located (always 0 for ISA bus).
    InterruptBus bus;
    /// The interrupt source relative to the bus, i.e. the ISA IRQ number (0-15).
    uint8_t source;
    /// The global system interrupt (GSI) to which the interrupt source is mapped.
    uint32_t globalSystemInterrupt;
    /// Flags describing the behavior of the interrupt source, as described by the `IntiFlag` enum.
    uint16_t flags;
} __attribute__ ((packed));

/// Describes a non-maskable interrupt (NMI) source.
/// These interrupts are typically used for critical events that cannot be masked or ignored.
/// All NMI sources must be parsed by the system and applied to the I/O APIC.
struct NmiSource {
    /// The header that prepends every APIC structure.
    ApicStructureHeader header;
    /// Flags describing the behavior of the NMI source, as described by the `IntiFlag` enum.
    uint16_t flags;
    /// The global system interrupt (GSI) of the NMI source.
    uint32_t globalSystemInterrupt;
} __attribute__ ((packed));

/// Describes the local APIC interrupt input (LINT) to which a non-maskable interrupt (NMI) is connected.
struct LocalApicNmi {
    /// The header that prepends every APIC structure.
    ApicStructureHeader header;
    /// The ACPI processor ID used to identify the CPU (Deprecated).
    uint8_t acpiProcessorId;
    /// Flags describing the behavior of the NMI source, as described by the `IntiFlag` enum.
    uint16_t flags;
    /// The local APIC interrupt input (LINT) number to which the NMI is connected.
    uint8_t localApicLint;
} __attribute__ ((packed));

/// The Multiple APIC Description Table (`Madt`) provides information about all
/// Advanced Programmable Interrupt Controllers (APICs) in the system. Its signature is "APIC".
/// Typically, this includes local APICs for each CPU core and one or multiple I/O APICs.
/// Furthermore, it contains information about interrupt source overrides and non-maskable interrupt sources.
/// All APIC structures in the `Madt` are listed directly after each other in memory right after this structure.
/// By using the `length` field of each structure, one can determine its size and iterate through the list.
/// For convenience, the `findStructures()` method is provided to find all structures of a specific type.
struct Madt {
    /// The header that prepends every ACPI table.
    SdtHeader header;
    /// The physical address at which each CPU core can access its local APIC registers via MMIO (Memory-Mapped I/O).
    uint32_t localApicAddress;
    /// Flags as described by the `MadtFlag` enum.
    uint32_t flags;

    /// Find all structures of the given type in this `Madt` and return pointers to them in an array.
    ///
    /// ### Example
    /// ```c++
    /// // `madt` has been obtained from the ACPI subsystem in kernel space or the file system in user space.
    /// const auto ioApics = madt.findStructures<Util::Hardware::Acpi::IoApic>(Util::Hardware::Acpi::IO_APIC);
    /// ```
    template <typename T>
    Array<const T*> findStructures(const ApicStructureType type) const {
        ArrayList<const T*> structures;
        const auto *pos = reinterpret_cast<const uint8_t*>(this) + sizeof(Madt);
        const auto *end = reinterpret_cast<const uint8_t*>(this) + header.length;

        while (pos < end) {
            const auto *header = reinterpret_cast<const ApicStructureHeader*>(pos);
            if (header->type == type) {
                structures.add(reinterpret_cast<const T*>(header));
            }

            pos += header->length;
        }

        return structures.toArray();
    }
} __attribute__ ((packed));

/// This table provides information about the High Precision Event Timer (HPET).
struct Hpet {
    /// The header that prepends every ACPI table.
    SdtHeader header;
    /// The hardware revision of the HPET.
    uint8_t hardwareRevision;
    /// Number of comparators in the first timer block.
    uint8_t comparatorCount: 5;
    /// Indicates the counter size (0 = 32-bit, 1 = 64-bit).
    uint8_t counterSize: 1;
    /// Reserved bit.
    uint8_t reserved: 1;
    /// Indicates whether the HPET supports legacy replacement (0 = no, 1 = yes).
    uint8_t legacy_replacement: 1;
    /// PCI vendor ID of the first timer block.
    uint16_t pciVendorId;
    /// Base address of the event timer block (1KiB large).
    GenericAddressStructure address;
    /// HPET sequence number (0-based), which is used to identify the HPET in systems with multiple HPETs.
    uint8_t hpetNumber;
    /// Minimum clock tick in femtoseconds (1 femtosecond = 10^-15 seconds).
    uint16_t minimumTick;
    /// Page protection flags for the HPET memory region. 0 = no protection, 1 = 4KiB page, 2 = 64KiB page
    uint8_t pageProtection;
} __attribute__((packed));

/// Enumeration of image types used in the Boot Graphics Resource Table (`Bgrt`).
enum ImageType : uint8_t {
    BITMAP = 0x00
};

/// The Boot Graphics Resource Table (`Bgrt`) provides information about the vendor-specific boot graphics image,
/// shown during BIOS initialization. It can be used to display the manufacturer's logo during the boot process,
/// providing a seemingly seamless transition from the BIOS to the operating system. Its signature is "BGRT".
/// The data pointed to by `imageAddress` is not raw pixel data, but rather a bitmap file. Information such as
/// the image size and bit depth can be found in the bitmap header (pointed to by `imageAddress`).
struct Bgrt {
    /// The header that prepends every ACPI table.
    SdtHeader header;
    /// BGRT version (must be 1).
    uint16_t version;
    /// Current image status.
    ///   - Bit [0]: 0 = image currently not displayed, 1 = image currently displayed
    ///   - Bit [2:1]: 00 = image not rotated, 01 = image rotated 90 degrees clockwise,
    ///                10 = image rotated 180 degrees, 11 = image rotated 270 degrees clockwise
    ///   - Bit [7:3]: Reserved, must be 0
    uint8_t status;
    /// Image type (must be 0 for `BITMAP`).
    uint8_t imageType;
    /// Physical address of the bitmap file in memory.
    uint64_t imageAddress;
    /// Display x-offset in pixels, relative to the top-left corner of the screen.
    uint32_t imageOffsetX;
    /// Display y-offset in pixels, relative to the top-left corner of the screen.
    uint32_t imageOffsetY;
} __attribute__ ((packed));

};

#endif
