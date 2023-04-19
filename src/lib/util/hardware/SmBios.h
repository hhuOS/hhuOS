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

#ifndef HHUOS_UTIL_SMBIOS_H
#define HHUOS_UTIL_SMBIOS_H

#include <cstdint>

namespace Util::Hardware {

class SmBios {

public:

    enum HeaderType : uint8_t {
        BIOS_INFORMATION = 0,
        SYSTEM_INFORMATION = 1,
        MAINBOARD_INFORMATION = 2,
        CHASSIS_INFORMATION = 3,
        PROCESSOR_INFORMATION = 4,
        CACHE_INFORMATION = 7,
        SYSTEM_SLOTS_INFORMATION = 9,
        PHYSICAL_MEMORY_ARRAY = 16,
        MEMORY_DEVICE_INFORMATION = 17,
        MEMORY_ARRAY_MAPPED_ADDRESS = 19,
        MEMORY_DEVICE_MAPPED_ADDRESS = 20,
        SYSTEM_BOOT_INFORMATION = 32,
        TERMINATE = 127
    };

    struct Info {
        uint8_t majorVersion;
        uint8_t minorVersion;
        uint32_t tableAddress;
        uint16_t tableLength;
    };

    struct TableHeader {
        HeaderType type;
        uint8_t length;
        uint16_t handle;

        [[nodiscard]] uint16_t calculateFullLength() const;
        [[nodiscard]] const char* getString(uint8_t number) const;
    } __attribute__ ((packed));

    struct BiosInformation {
        TableHeader header;
        uint8_t vendorString;
        uint8_t versionString;
        uint16_t startAddressSegment;
        uint8_t releaseDateString;
        uint8_t romSize;
        uint64_t characteristics;
        uint16_t characteristicsExtension;
        uint8_t majorVersion;
        uint8_t minorVersion;
        uint8_t embeddedControllerMajorVersion;
        uint8_t embeddedControllerMinorVersion;
        uint16_t extendedRomSize;

        [[nodiscard]] const char* getVendorName() const {
            return header.getString(vendorString);
        }

        [[nodiscard]] const char* getVersion() const {
            return header.getString(versionString);
        }

        [[nodiscard]] const char* getReleaseDate() const {
            return header.getString(releaseDateString);
        }
    } __attribute__ ((packed));

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    SmBios() = delete;

    /**
     * Copy Constructor.
     */
    SmBios(const SmBios &other) = delete;

    /**
     * Assignment operator.
     */
    SmBios &operator=(const SmBios &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~SmBios() = delete;
};

}

#endif
