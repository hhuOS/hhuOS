/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_COLORGRAPHICSARRAYPROVIDER_H
#define HHUOS_COLORGRAPHICSARRAYPROVIDER_H

#include <lib/util/memory/Address.h>
#include <device/cpu/IoPort.h>
#include "TerminalProvider.h"

namespace Device::Graphic {

class ColorGraphicsArrayProvider : public TerminalProvider {

public:

    PROTOTYPE_IMPLEMENT_CLONE(ColorGraphicsArrayProvider);

    /**
     * Default Constructor.
     */
    explicit ColorGraphicsArrayProvider(bool prototypeInstance = false);

    /**
     * Copy constructor.
     */
    ColorGraphicsArrayProvider(const ColorGraphicsArrayProvider &other) = delete;

    /**
     * Assignment operator.
     */
    ColorGraphicsArrayProvider &operator=(const ColorGraphicsArrayProvider &other) = delete;

    /**
     * Destructor.
     */
    ~ColorGraphicsArrayProvider() override = default;

    /**
     * Check if a CGA compatible graphics card is available and this driver can be used.
     *
     * @return true, if this driver can be used
     */
    [[nodiscard]] static bool isAvailable();

    /**
     * Overriding function from TerminalProvider.
     */
    Util::Graphic::Terminal& initializeTerminal(ModeInfo &modeInfo) override;

    /**
     * Overriding function from TerminalProvider.
     */
    void destroyTerminal(Util::Graphic::Terminal &terminal) override;

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Data::Array<ModeInfo> getAvailableModes() const override;

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] uint32_t getVideoMemorySize() const override;

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Memory::String getDeviceName() const override;

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Memory::String getClassName() override;

private:

    enum BiosFunction : uint16_t {
        CHECK_VIDEO_CARD = 0x1a00,
        SET_CURSOR_SHAPE = 0x0100
    };

    enum VideoCardType : uint8_t {
        NO_DISPLAY = 0x00,
        MONOCHROME = 0x01,
        CGA_COLOR = 0x02,
        EGA_COLOR = 0x04,
        EGA_MONOCHROME = 0x05,
        PGA_COLOR = 0x06,
        VGA_MONOCHROME = 0x07,
        VGA_COLOR = 0x08,
        MCGA_COLOR_DIGITAL = 0x0a,
        MCGA_MONOCHROME = 0x0b,
        MCGA_COLOR = 0x0c,
        UNKNOWN = 0xff
    };

    uint32_t videoMemorySize = 0;
    Util::Memory::String deviceName = "Unknown";
    Util::Data::Array<ModeInfo> supportedModes;

    static const constexpr uint16_t CURSOR_SHAPE_OPTIONS = 0x0e0f;
    static const constexpr char *CLASS_NAME = "Device::Graphic::ColorGraphicsArrayProvider";
};

}

#endif
