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

#ifndef HHUOS_TERMINALPROVIDER_H
#define HHUOS_TERMINALPROVIDER_H

#include <cstdint>

#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"

namespace Util {
namespace Graphic {
class Terminal;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class TerminalProvider : public Util::Reflection::Prototype {

public:
    /**
     * Information about a resolution.
     *
     * @var columns Horizontal resolution
     * @var rows Vertical resolution
     * @var colorDepth Color depth in bits
     * @var modeNumber Unique number, used to identify the mode. May be ignored if not needed.
     */
    struct ModeInfo {
        uint16_t columns;
        uint16_t rows;
        uint8_t colorDepth;
        uint16_t modeNumber;

        bool operator==(const ModeInfo &other) const {
            return columns == other.columns && rows == other.rows &&
                   colorDepth == other.colorDepth && modeNumber == other.modeNumber;
        }

        bool operator!=(const ModeInfo &other) const {
            return columns != other.columns || rows != other.rows ||
                   colorDepth != other.colorDepth || modeNumber != other.modeNumber;
        }
    };

    /**
     * Default Constructor.
     */
    TerminalProvider() = default;

    /**
     * Copy Constructor.
     */
    TerminalProvider(const TerminalProvider &other) = delete;

    /**
     * Assignment operator.
     */
    TerminalProvider &operator=(const TerminalProvider &other) = delete;

    /**
     * Destructor.
     */
    ~TerminalProvider() override = default;

    /**
     * Create a terminal with a given mode info.
     *
     * @param modeInfo Information about the desired terminal resolution
     * @param filename The name of the file, representing the created terminal in '/device/'
     */
    void initializeTerminal(const ModeInfo &modeInfo, const Util::Memory::String &filename);

    /**
     * Get all available graphics modes.
     *
     * @return An array, containing all available graphics modes
     */
    [[nodiscard]] virtual Util::Data::Array<ModeInfo> getAvailableModes() const = 0;

    /**
     * Search for a graphics mode, that suits the given parameters best.
     *
     * @param columns The desired width
     * @param rows The desired height
     * @param colorDepth The desired color depth
     * @return The found ModeInfo struct
     */
    [[nodiscard]] ModeInfo searchMode(uint16_t columns, uint16_t rows, uint8_t colorDepth) const;

    /**
     * Overriding function from Prototype.
     */
    [[nodiscard]] Util::Memory::String getClassName() const override = 0;

protected:

    virtual Util::Graphic::Terminal* initializeTerminal(const ModeInfo &modeInfo) = 0;
};

}

#endif
