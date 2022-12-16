/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/data/ArrayList.h"
#include "device/graphic/lfb/LinearFrameBufferProvider.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

namespace Kernel {
class Logger;
}  // namespace Kernel

namespace Device::Graphic {

class VesaBiosExtensions : public LinearFrameBufferProvider {

public:

    /**
     * Default Constructor.
     */
    explicit VesaBiosExtensions(bool prototypeInstance = false);

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
     */
    ~VesaBiosExtensions() override = default;

    PROTOTYPE_IMPLEMENT_CLONE(VesaBiosExtensions)

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Device::Graphic::VesaBiosExtensions")

    /**
     * Check if a VESA compatible graphics card is available and this driver can be used.
     *
     * @return true, if this driver can be used
     */
    [[nodiscard]] static bool isAvailable();

    /**
     * Overriding virtual function from LinearFrameBufferProvider.
     */
    [[nodiscard]] Util::Data::Array<ModeInfo> getAvailableModes() const override;

protected:
    /**
     * Overriding virtual function from LinearFrameBufferProvider.
     */
    Util::Graphic::LinearFrameBuffer* initializeLinearFrameBuffer(const ModeInfo &modeInfo) override;

private:
    /**
	 * Information about a VBE device.
	 * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
	 */
    struct VbeInfo {
        char signature[4] = {'V', 'B', 'E', '2'};
        uint16_t version{};         // VBE version; high byte is major version, low byte is minor version
        uint32_t oem{};             // segment:offset pointer to OEM
        uint32_t capabilities{};    // bitfield that describes card capabilitiesPointer
        uint16_t video_modes[2]{};  // segment:offset pointer to list of supported video modes
        uint16_t video_memory{};    // amount of video memory in 64KB blocks
        uint16_t software_rev{};    // software revision
        uint16_t vendor[2]{};       // segment:offset to card vendor string
        uint16_t product_name[2]{}; // segment:offset to card model name
        uint16_t product_rev[2]{};  // segment:offset pointer to product revision
        char reserved[222]{};       // reserved for future expansion
        char oem_data[256]{};       // OEM BIOSes store their strings in this area
    } __attribute__((packed));

    /**
     * Information about a VBE graphics mode.
     * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
     */
    struct VbeModeInfo {
        uint16_t attributes;
        uint8_t winA, winB;
        uint16_t granularity;
        uint16_t winsize;
        uint16_t segmentA, segmentB;
        uint16_t realFctPtr[2];
        uint16_t pitch;          // bytes per Scanline

        uint16_t Xres, Yres;
        uint8_t Wchar, Ychar, planes, bpp, banks;
        uint8_t memory_model, bank_size, image_pages;
        uint8_t reserved0;

        uint8_t red_mask, red_position;
        uint8_t green_mask, green_position;
        uint8_t blue_mask, blue_position;
        uint8_t rsv_mask, rsv_position;
        uint8_t directcolor_attributes;

        uint32_t physbase;       // address of the linear framebuffer
        uint32_t reserved1;
        uint16_t reserved2;
    } __attribute__((packed));

    enum BiosFunction : uint16_t {
        GET_VBE_INFO = 0x4f00,
        GET_MODE_INFO = 0x4f01,
        SET_MODE = 0x4f02,
        GET_CURRENT_MODE = 0x4f03
    };

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
     * Get information about the VBE device from the BIOS.
     *
     * @return A pointer to the VBE info struct
     */
    static VbeInfo getVbeInfo();

    /**
     * Get information about a specific VBE graphics mode from the BIOS.
     *
     * @param mode The mode number
     * @return A pointer to the mode info struct
     */
    static VesaBiosExtensions::VbeModeInfo getModeInfo(uint16_t mode);

    /**
	 * Set the VBE device to a given mode.
	 *
	 * @param mode The mode
	 * @return Whether the mode has been set successfully
	 */
    static void setMode(uint16_t mode);

    Util::Data::ArrayList<ModeInfo> supportedModes;

    static Kernel::Logger log;

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
