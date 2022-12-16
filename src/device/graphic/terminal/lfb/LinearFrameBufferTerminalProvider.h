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

#ifndef HHUOS_LINEARFRAMEBUFFERTERMINALPROVIDER_H
#define HHUOS_LINEARFRAMEBUFFERTERMINALPROVIDER_H

#include "lib/util/data/Array.h"
#include "device/graphic/terminal/TerminalProvider.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/file/File.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"
#include "lib/util/graphic/Terminal.h"

namespace Util {
namespace Graphic {
class Font;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class LinearFrameBufferTerminalProvider : public TerminalProvider {

public:
    /**
     * Default Constructor.
     */
    explicit LinearFrameBufferTerminalProvider(Util::File::File &lfbFile, Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_FONT, char cursor = static_cast<char>(219));

    /**
     * Copy Constructor.
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

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Device::Graphic::LinearFrameBufferTerminalProvider")

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

    Util::File::File lfbFile;
    Util::Graphic::Font &font;
    char cursor;

    ModeInfo mode{};
};

}

#endif
