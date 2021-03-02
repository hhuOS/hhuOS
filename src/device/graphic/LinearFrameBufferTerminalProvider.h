/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_LINEARFRAMEBUFFERTERMINALPROVIDER_H
#define HHUOS_LINEARFRAMEBUFFERTERMINALPROVIDER_H

#include <util/data/Array.h>
#include <device/graphic/TerminalProvider.h>
#include <device/graphic/LinearFrameBufferProvider.h>
#include "util/graphic/Font.h"
#include "util/graphic/Fonts.h"

namespace Device::Graphic {

class LinearFrameBufferTerminalProvider : public TerminalProvider {

public:
    /**
     * Default Constructor.
     */
    explicit LinearFrameBufferTerminalProvider(LinearFrameBufferProvider &lfbProvider, Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_FONT, char cursor = '_');

    /**
     * Copy constructor.
     */
    LinearFrameBufferTerminalProvider(const LinearFrameBufferTerminalProvider &other) = delete;

    /**
     * Assignment operator.
     */
    LinearFrameBufferTerminalProvider &operator=(const LinearFrameBufferTerminalProvider &other) = delete;

    /**
     * Destructor.
     */
    ~LinearFrameBufferTerminalProvider() override = default;

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
    [[nodiscard]] Util::Memory::String getVendorName() const override;

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Memory::String getDeviceName() const override;

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Memory::String getClassName() override;

private:

    LinearFrameBufferProvider &lfbProvider;
    Util::Graphic::Font &font;
    char cursor;

    Util::Data::Array<ModeInfo> supportedModes;

    static const constexpr char *CLASS_NAME = "Device::Graphic::ColorGraphicsArray";
};

}

#endif
