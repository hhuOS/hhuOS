/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstdint>

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
    Util::System::out.setNumberPadding(2);
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

    Util::System::out.setNumberPadding(0);
}

void decodeBiosInformation(const Util::Hardware::SmBios::BiosInformation &table) {
    Util::System::out << "\tVendor: " << table.getVendorName() << Util::Io::PrintStream::endl;
    Util::System::out << "\tVersion: " << table.getVersion() << Util::Io::PrintStream::endl;
    Util::System::out << "\tRelease Date: " << table.getReleaseDate() << Util::Io::PrintStream::endl;
    Util::System::out << "\tAddress: 0x" << Util::Io::PrintStream::hex << (table.startAddressSegment << 4) << Util::Io::PrintStream::dec << Util::Io::PrintStream::endl;
    Util::System::out << "\tRuntime Size: " << table.calculateRuntimeSize() / 1024 << " KiB" << Util::Io::PrintStream::endl;
    Util::System::out << "\tROM Size: " << table.calculateRomSize() / 1024 << " KiB" << Util::Io::PrintStream::endl;
    Util::System::out << "\tCharacteristics: " << Util::Io::PrintStream::endl;

    if (!(table.characteristics & (1 << 2))) {
        if (table.characteristics & (1 << 3)) {
            Util::System::out << "\t\tISA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 4)) {
            Util::System::out << "\t\tMCA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 5)) {
            Util::System::out << "\t\tEISA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 6)) {
            Util::System::out << "\t\tPCI is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 7)) {
            Util::System::out << "\t\tPC card (PCMCIA) is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 8)) {
            Util::System::out << "\t\tPlug and Play is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 9)) {
            Util::System::out << "\t\tAPM is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 10)) {
            Util::System::out << "\t\tBIOS is upgradeable" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 11)) {
            Util::System::out << "\t\tBIOS shadowing is allowed" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 12)) {
            Util::System::out << "\t\tVL-VESA is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 13)) {
            Util::System::out << "\t\tESCD support is available" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 14)) {
            Util::System::out << "\t\tBoot from CD is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 15)) {
            Util::System::out << "\t\tSelectable boot is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 16)) {
            Util::System::out << "\t\tBIOS ROM is socketed" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 17)) {
            Util::System::out << "\t\tBoot from PC card (PCMCIA) is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 18)) {
            Util::System::out << "\t\tEDD is supported" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 19)) {
            Util::System::out << "\t\tJapanese floppy for NEC 9800 1.2 MB is supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 20)) {
            Util::System::out << "\t\tJapanese floppy for Toshiba 1.2 MB is supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 21)) {
            Util::System::out << "\t\t5.25/360 kB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 22)) {
            Util::System::out << "\t\t5.25/1.2 MB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 23)) {
            Util::System::out << "\t\t3.5/720 kB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 24)) {
            Util::System::out << "\t\t3.5/2.88 MB floppy services are supported (int 13h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 25)) {
            Util::System::out << "\t\tPrint screen service is supported (int 5h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 26)) {
            Util::System::out << "\t\t8042 keyboard services are supported (int 9h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 27)) {
            Util::System::out << "\t\tSerial services are supported (int 14h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 28)) {
            Util::System::out << "\t\tPrinter services are supported (int 17h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 29)) {
            Util::System::out << "\t\tCGA/Mono video services are supported (int 10h)" << Util::Io::PrintStream::endl;
        }
        if (table.characteristics & (1 << 30)) {
            Util::System::out << "\t\tNEC PC-98" << Util::Io::PrintStream::endl;
        }

        if (version >= 2.4) {
            if (table.characteristicsExtension1 & (1 << 0)) {
                Util::System::out << "\t\tACPI is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 1)) {
                Util::System::out << "\t\tUSB Legacy is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 2)) {
                Util::System::out << "\t\tAGP is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 3)) {
                Util::System::out << "\t\tI2O boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 4)) {
                Util::System::out << "\t\tLS-120 SuperDisk boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 5)) {
                Util::System::out << "\t\tATAPI ZIP drive boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 6)) {
                Util::System::out << "\t\t1394 boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension1 & (1 << 7)) {
                Util::System::out << "\t\tSmart battery is supported" << Util::Io::PrintStream::endl;
            }

            if (table.characteristicsExtension2 & (1 << 0)) {
                Util::System::out << "\t\tBIOS boot specification is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & (1 << 1)) {
                Util::System::out << "\t\tFunction key-initiated network boot is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & (1 << 2)) {
                Util::System::out << "\t\tTargeted content distribution is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & (1 << 3)) {
                Util::System::out << "\t\tUEFI is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & (1 << 4)) {
                Util::System::out << "\t\tSystem is a virtual machine" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & (1 << 5)) {
                Util::System::out << "\t\tManufacturing mode is supported" << Util::Io::PrintStream::endl;
            }
            if (table.characteristicsExtension2 & (1 << 6)) {
                Util::System::out << "\t\tManufacturing mode is enabled" << Util::Io::PrintStream::endl;
            }
        }
    }

    if (version >= 2.4) {
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
        targetType = static_cast<Util::Hardware::SmBios::HeaderType>(Util::String::parseInt(argumentParser.getArgument("type")));
    }

    auto versionStream = Util::Io::FileInputStream(versionFile);
    auto versionString = versionStream.readLine();
    auto versionSplit = versionString.split(".");
    uint8_t majorVersion = Util::String::parseInt(versionSplit[0]);
    uint8_t minorVersion = Util::String::parseInt(versionSplit[1]);
    version = majorVersion + (minorVersion / 10.0);

    Util::System::out << "SMBIOS " << majorVersion << "." << minorVersion << " present." << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    auto tableDirectory = Util::Io::File("/device/smbios/tables");
    for (const auto &tableId : tableDirectory.getChildren()) {
        auto tableType = Util::String::parseInt(tableId.split("-")[0]);
        if (targetType != Util::Hardware::SmBios::END_OF_TABLE && targetType != tableType) {
            continue;
        }

        auto tableFile = Util::Io::File(tableDirectory.getCanonicalPath() + "/" + tableId);
        auto tableStream = Util::Io::FileInputStream(tableFile);

        auto length = tableFile.getLength();
        auto *tableBuffer = new uint8_t[length];
        tableStream.read(tableBuffer, 0, length);

        auto *tableHeader = reinterpret_cast<Util::Hardware::SmBios::TableHeader*>(tableBuffer);
        Util::System::out.setNumberPadding(4);
        Util::System::out << "Handle 0x" << Util::Io::PrintStream::hex << tableHeader->handle << Util::Io::PrintStream::dec;
        Util::System::out.setNumberPadding(2);
        Util::System::out << ", DMI type " << tableHeader->type << ", " << tableHeader->length << " bytes" << Util::Io::PrintStream::endl;
        Util::System::out.setNumberPadding(0);

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