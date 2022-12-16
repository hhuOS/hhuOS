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

#ifndef HHUOS_COLORGRAPHICSADAPTERPROVIDER_H
#define HHUOS_COLORGRAPHICSADAPTERPROVIDER_H

#include <cstdint>

#include "device/graphic/terminal/TerminalProvider.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"
#include "lib/util/graphic/Terminal.h"

namespace Kernel {
class Logger;
}  // namespace Kernel

namespace Device::Graphic {

class ColorGraphicsAdapterProvider : public TerminalProvider {

public:

    /**
     * Default Constructor.
     */
    explicit ColorGraphicsAdapterProvider(bool prototypeInstance = false);

    /**
     * Copy Constructor.
     */
    ColorGraphicsAdapterProvider(const ColorGraphicsAdapterProvider &other) = delete;

    /**
     * Assignment operator.
     */
    ColorGraphicsAdapterProvider &operator=(const ColorGraphicsAdapterProvider &other) = delete;

    /**
     * Destructor.
     */
    ~ColorGraphicsAdapterProvider() override = default;

    PROTOTYPE_IMPLEMENT_CLONE(ColorGraphicsAdapterProvider);

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Device::Graphic::ColorGraphicsAdapterProvider")

    /**
     * Check if a CGA compatible graphics card is available and this driver can be used.
     *
     * @return true, if this driver can be used
     */
    [[nodiscard]] static bool isAvailable();

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Data::Array<ModeInfo> getAvailableModes() const override;

protected:
    /**
     * Overriding function from TerminalProvider.
     */
    Util::Graphic::Terminal* initializeTerminal(const ModeInfo &modeInfo) override;

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

    static VideoCardType getVideoCardType();

    static Util::Memory::String getVideoCardTypeAsString(VideoCardType cardType);

    Util::Data::Array<ModeInfo> supportedModes{{40, 25, 4, 0x01},
                                               {80, 25, 4, 0x03}};

    static Kernel::Logger log;

    static const constexpr uint16_t CURSOR_SHAPE_OPTIONS = 0x0e0f;
};

}

#endif
