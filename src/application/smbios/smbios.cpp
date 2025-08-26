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

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/hardware/SmBios.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

double version = 0;

static const constexpr char *tableNames[]{
    "BIOS Information",
    "System Information",
    "Baseboard (or Module) Information",
    "System Enclosure or Chassis",
    "Processor Information",
    "Memory Controller Information",
    "Memory Module Information",
    "Cache Information",
    "Port Connector Information",
    "System Slots",
    "Onboard Devices Information",
    "OEM Strings",
    "System Configuration Options",
    "BIOS Language Information",
    "Group Associations",
    "System Event Log",
    "Physical Memory Array",
    "Memory Device",
    "32-Bit Memory Error Information",
    "Memory Array Mapped Address",
    "Memory Device Mapped Address",
    "Built-in Pointing Device",
    "Portable Battery",
    "System Reset",
    "Hardware Security",
    "System Power Controls",
    "Voltage Probe",
    "Cooling Device",
    "Temperature Probe",
    "Electrical Current Probe",
    "Out-of-Band Remote Access",
    "Boot Integrity Services (BIS) Entry Point",
    "System Boot Information",
    "64-Bit Memory Error Information",
    "Management Device",
    "Management Device Component",
    "Management Device Threshold Data",
    "Memory Channel",
    "IPMI Device Information",
    "System Power Supply",
    "Additional Information",
    "Onboard Devices Extended Information",
    "Management Controller Host Interface",
    "TPM Device",
    "Processor Additional Information",
    "Firmware Inventory Information",
    "String Property",
};

void dumpTable(const Util::Hardware::SmBios::TableHeader &table) {
    Util::System::out.setIntegerPrecision(2);
    Util::System::out << "\tHeader and Data:" << Util::Io::PrintStream::hex;

    for (uint32_t i = 0; i < table.length; i++) {
        if (i % 16 == 0) {
            Util::System::out << Util::Io::PrintStream::endl << "\t\t";
        }

        Util::System::out << reinterpret_cast<const uint8_t*>(&table)[i] << " ";
    }
    Util::System::out << Util::Io::PrintStream::dec << Util::Io::PrintStream::endl;

    auto stringCount = table.calculateStringCount();
    if (stringCount == 0) {
        return;
    }

    Util::System::out << "\tStrings:" << Util::Io::PrintStream::endl;
    for (uint32_t i = 0; i < stringCount; i++) {
        Util::System::out << "\t\t" << table.getString(i + 1) << Util::Io::PrintStream::endl;
    }

    Util::System::out.setIntegerPrecision(0);
}

void decodeBiosInformation(const Util::Hardware::SmBios::BiosInformation &table) {
    Util::System::out << "\tVendor: " << table.getVendorName() << Util::Io::PrintStream::endl;
    Util::System::out << "\tVersion: " << table.getVersion() << Util::Io::PrintStream::endl;
    Util::System::out << "\tRelease Date: " << table.getReleaseDate() << Util::Io::PrintStream::endl;
    Util::System::out << "\tAddress: 0x" << Util::Io::PrintStream::hex << (table.startAddressSegment << 4) << Util::Io::PrintStream::dec << Util::Io::PrintStream::endl;
    Util::System::out << "\tRuntime Size: " << table.calculateRuntimeSize() / 1024 << " KiB" << Util::Io::PrintStream::endl;
    Util::System::out << "\tROM Size: " << table.calculateRomSize() / 1024 << " KiB" << Util::Io::PrintStream::endl;
    Util::System::out << "\tCharacteristics: " << Util::Io::PrintStream::endl;

    if (!(table.characteristics & Util::Hardware::SmBios::CHARACTERISTICS_NOT_SUPPORTED)) {
        if (table.characteristics & Util::Hardware::SmBios::ISA_SUPPORTED) {
            Util::System::out << "\t\tISA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::MCA_SUPPORTED) {
            Util::System::out << "\t\tMCA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::EISA_SUPPORTED) {
            Util::System::out << "\t\tEISA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::PCI_SUPPORTED) {
            Util::System::out << "\t\tPCI is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::PCMCIA_SUPPORTED) {
            Util::System::out << "\t\tPC card (PCMCIA) is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::PLUG_AND_PLAY_SUPPORTED) {
            Util::System::out << "\t\tPlug and Play is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::APM_SUPPORTED) {
            Util::System::out << "\t\tAPM is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::FIRMWARE_UPGRADEABLE) {
            Util::System::out << "\t\tBIOS firmware is upgradeable" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::FIRMWARE_SHADOWING_ALLOWED) {
            Util::System::out << "\t\tBIOS firmware shadowing is allowed" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::VL_VESA_SUPPORTED) {
            Util::System::out << "\t\tVL-VESA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::ESCD_SUPPORTED) {
            Util::System::out << "\t\tESCD support is available" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::CD_BOOT_SUPPORTED) {
            Util::System::out << "\t\tBoot from CD is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::SELECTABLE_BOOT_SUPPORTED) {
            Util::System::out << "\t\tSelectable boot is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::FIRMWARE_ROM_SOCKETED) {
            Util::System::out << "\t\tBIOS firmware ROM is socketed" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::PCMCIA_BOOT_SUPPORTED) {
            Util::System::out << "\t\tBoot from PC card (PCMCIA) is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::EDD_SUPPORTED) {
            Util::System::out << "\t\tEDD is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT13_NEC9800_FLOPPY_SUPPORTED) {
            Util::System::out << "\t\tJapanese floppy for NEC 9800 1.2 MB is supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT13_TOSHIBA_FLOPPY_SUPPORTED) {
            Util::System::out << "\t\tJapanese floppy for Toshiba 1.2 MB is supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT13_360K_FLOPPY_SUPPORTED) {
            Util::System::out << "\t\t5.25/360 kB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT13_1_2M_FLOPPY_SUPPORTED) {
            Util::System::out << "\t\t5.25/1.2 MB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT13_720K_FLOPPY_SUPPORTED) {
            Util::System::out << "\t\t3.5/720 kB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT13_2_88M_FLOPPY_SUPPORTED) {
            Util::System::out << "\t\t3.5/2.88 MB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT5_PRINT_SCREEN_SERVICE_SUPPORTED) {
            Util::System::out << "\t\tPrint screen service is supported (int 5h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT9_KEYBOARD_SERVICE_SUPPORTED) {
            Util::System::out << "\t\t8042 keyboard services are supported (int 9h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT14_SERIAL_SERVICE_SUPPORTED) {
            Util::System::out << "\t\tSerial services are supported (int 14h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT17_PRINTER_SERVICE_SUPPORTED) {
            Util::System::out << "\t\tPrinter services are supported (int 17h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::INT10_VIDEO_SERVICE_SUPPORTED) {
            Util::System::out << "\t\tCGA/Mono video services are supported (int 10h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & Util::Hardware::SmBios::NEC_PC98) {
            Util::System::out << "\t\tNEC PC-98" << Util::Io::PrintStream::endl;
        }

        if (table.header.length > 18) {
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::ACPI_SUPPORTED) {
                Util::System::out << "\t\tACPI is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::USB_LEGACY_SUPPORTED) {
                Util::System::out << "\t\tUSB Legacy is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::AGP_SUPPORTED) {
                Util::System::out << "\t\tAGP is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::I2O_BOOT_SUPPORTED) {
                Util::System::out << "\t\tI2O boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::LS120_BOOT_SUPPORTED) {
                Util::System::out << "\t\tLS-120 SuperDisk boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::ATAPI_ZIP_DRIVE_BOOT_SUPPORTED) {
                Util::System::out << "\t\tATAPI ZIP drive boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::FIREWIRE_BOOT_SUPPORTED) {
                Util::System::out << "\t\t1394 boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & Util::Hardware::SmBios::SMART_BATTERY_SUPPORTED) {
                Util::System::out << "\t\tSmart battery is supported" << Util::Io::PrintStream::endl;
            }
        }

        if (table.header.length > 19) {
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::BIOS_BOOT_SPECIFICATION_SUPPORTED) {
                Util::System::out << "\t\tBIOS boot specification is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::FUNCTION_KEY_INITIATED_NETWORK_BOOT_SUPPORTED) {
                Util::System::out << "\t\tFunction key-initiated network boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::TARGETED_CONTENT_DISTRIBUTION_ENABLED) {
                Util::System::out << "\t\tTargeted content distribution is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::UEFI_SUPPORTED) {
                Util::System::out << "\t\tUEFI is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::VIRTUAL_MACHINE) {
                Util::System::out << "\t\tSystem is a virtual machine" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::MANUFACTURING_MODE_SUPPORTED) {
                Util::System::out << "\t\tManufacturing mode is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & Util::Hardware::SmBios::MANUFACTURING_MODE_ENABLED) {
                Util::System::out << "\t\tManufacturing mode is enabled" << Util::Io::PrintStream::endl;
            }
        }
    }

    if (table.header.length > 20) {
        Util::System::out << "\tBIOS Revision: " << table.majorVersion << "." << table.minorVersion << Util::Io::PrintStream::endl;
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("type", false, "t");
    argumentParser.setHelpText("Decode SMBIOS tables.\n"
                               "Usage: smbios [OPTION]...\n"
                               "Options:\n"
                               "  -t, --type: Only display the entries of the given type\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto versionFile = Util::Io::File("/device/smbios/version");
    if (!versionFile.exists()) {
        Util::System::error << "SMBIOS is not available on this system!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto targetType = Util::Hardware::SmBios::END_OF_TABLE;
    if (argumentParser.hasArgument("type")) {
        targetType = static_cast<Util::Hardware::SmBios::HeaderType>(Util::String::parseNumber<uint8_t>(argumentParser.getArgument("type")));
    }

    bool endOfFile = false;
    auto versionStream = Util::Io::FileInputStream(versionFile);
    auto versionString = versionStream.readLine(endOfFile);
    auto versionSplit = versionString.split(".");
    auto majorVersion = Util::String::parseNumber<uint8_t>(versionSplit[0]);
    auto minorVersion = Util::String::parseNumber<uint8_t>(versionSplit[1]);
    version = majorVersion + (minorVersion / 10.0);

    Util::System::out << "SMBIOS " << majorVersion << "." << minorVersion << " present." << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    auto tableDirectory = Util::Io::File("/device/smbios/tables");
    for (auto &tableFile : tableDirectory.getChildren()) {
        auto tableType = Util::String::parseNumber<uint32_t>(tableFile.getName().split("-")[0]);
        if (targetType != Util::Hardware::SmBios::END_OF_TABLE && targetType != tableType) {
            continue;
        }

        auto tableStream = Util::Io::FileInputStream(tableFile);

        auto length = tableFile.getLength();
        auto *tableBuffer = new uint8_t[length];
        tableStream.read(tableBuffer, 0, length);

        auto *tableHeader = reinterpret_cast<Util::Hardware::SmBios::TableHeader*>(tableBuffer);
        Util::System::out.setIntegerPrecision(4);
        Util::System::out << "Handle 0x" << Util::Io::PrintStream::hex << tableHeader->handle << Util::Io::PrintStream::dec;
        Util::System::out.setIntegerPrecision(2);
        Util::System::out << ", DMI type " << tableHeader->type << ", " << tableHeader->length << " bytes" << Util::Io::PrintStream::endl;
        Util::System::out.setIntegerPrecision(0);

        if (tableHeader->type <= Util::Hardware::SmBios::STRING_PROPERTY) {
            Util::System::out << tableNames[tableHeader->type] << Util::Io::PrintStream::endl;
        } else if (tableHeader->type == Util::Hardware::SmBios::INACTIVE) {
            Util::System::out << "Inactive" << Util::Io::PrintStream::endl;
        } else {
            Util::System::out << "OEM-specific Type" << Util::Io::PrintStream::endl;
        }

        switch (tableHeader->type) {
            case Util::Hardware::SmBios::BIOS_INFORMATION:
                decodeBiosInformation(*reinterpret_cast<const Util::Hardware::SmBios::BiosInformation*>(tableHeader));
                break;
            default:
                dumpTable(*tableHeader);
        }

        Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    return 0;
}