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

#ifndef HHUOS_VESABIOSEXTENSIONS_H
#define HHUOS_VESABIOSEXTENSIONS_H

#include <cstdint>

#include "lib/util/collection/Array.h"

namespace Device::Graphic {

class VesaBiosExtensions {

public:

    enum MemoryModel : uint8_t {
        TEXT_MODE = 0x00,
        CGA_GRAPHICS = 0x01,
        HERCULES_GRAPHICS = 0x02,
        PLANAR = 0x03,
        PACKED_PIXEL = 0x04,
        NON_CHAIN = 0x05,
        DIRECT_COLOR = 0x06,
        YUV = 0x07
    };

    /**
     * Information about a VBE device.
     * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
     */
    struct DeviceInfo {
        char signature[4]{'V', 'B', 'E', '2'};
        uint16_t version; // VBE version; high byte is major version, low byte is minor version
        uint32_t oem; // segment:offset pointer to OEM
        uint32_t capabilities; // bitfield that describes card capabilitiesPointer
        uint16_t videoModes[2]; // segment:offset pointer to list of supported video modes
        uint16_t videoMemory; // amount of video memory in 64KB blocks
        uint16_t softwareRevision; // software revision
        uint16_t vendor[2]; // segment:offset to card vendor string
        uint16_t productName[2]; // segment:offset to card model name
        uint16_t productRevision[2]; // segment:offset pointer to product revision
        char reserved[222]; // reserved for future expansion
        char oemData[256]; // OEM BIOSes store their strings in this area

        bool isValid();
    } __attribute__((packed));

    struct UsableMode {
        uint16_t resolutionX;
        uint16_t resolutionY;
        uint8_t colorDepth;
        uint16_t pitch;
        uint16_t modeNumber;

        bool operator!=(const UsableMode &other) const;
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    VesaBiosExtensions() = delete;

    /**
     * Copy Constructor.
     */
    VesaBiosExtensions(const VesaBiosExtensions &other) = delete;

    /**
     * Assignment operator.
     */
    VesaBiosExtensions &operator=(const VesaBiosExtensions &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~VesaBiosExtensions() = delete;

    static DeviceInfo getDeviceInfo();

    [[nodiscard]] static Util::Array<UsableMode> getAvailableModes();

    static void setMode(uint16_t mode);

private:

    enum BiosFunction : uint16_t {
        GET_VBE_INFO = 0x4f00,
        GET_MODE_INFO = 0x4f01,
        SET_MODE = 0x4f02,
        GET_CURRENT_MODE = 0x4f03
    };

    /**
     * Information about a VBE graphics mode.
     * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
     */
    struct ModeInfo {
        uint16_t attributes;
        uint8_t winA, winB;
        uint16_t granularity;
        uint16_t windowSize;
        uint16_t segmentA, segmentB;
        uint16_t realFctPtr[2];
        uint16_t pitch;

        uint16_t resX;
        uint16_t resY;
        uint8_t wChar;
        uint8_t yChar;
        uint8_t planes;
        uint8_t bpp;
        uint8_t banks;
        MemoryModel memoryModel;
        uint8_t bankSize, imagePanes;
        uint8_t reserved0;

        uint8_t redMask, redPosition;
        uint8_t greenMask, greenPosition;
        uint8_t blueMask, bluePosition;
        uint8_t rsvMask, rsvPosition;
        uint8_t directColorAttributes;

        uint32_t physicalAddress;
        uint32_t reserved1;
        uint16_t reserved2;
    } __attribute__((packed));

    /**
     * Get information about a specific VBE graphics mode from the BIOS.
     *
     * @param mode The mode number
     * @return A pointer to the mode info struct
     */
    static VesaBiosExtensions::ModeInfo getModeInfo(uint16_t mode);

    static const constexpr uint32_t VBE_CONTROLLER_INFO_SIZE = 512;
    static const constexpr uint32_t VBE_MODE_INFO_SIZE = 256;
    static const constexpr uint16_t BIOS_CALL_RETURN_CODE_SUCCESS = 0x004f;
    static const constexpr uint16_t MODE_LIST_END_MARKER = 0xffff;
    static const constexpr uint16_t MODE_NUMBER_LFB_BIT = 1 << 14;
    static const constexpr uint16_t MODE_ATTRIBUTES_HARDWARE_SUPPORT_BIT = 1 >> 0;
    static const constexpr uint16_t MODE_ATTRIBUTES_LFB_BIT = 1 << 7;
    static const constexpr char *VESA_SIGNATURE = "VESA";
};

}

#endif
