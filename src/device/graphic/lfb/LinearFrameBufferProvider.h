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

#ifndef HHUOS_LINEARFRAMEBUFFERPROVIDER_H
#define HHUOS_LINEARFRAMEBUFFERPROVIDER_H

#include <cstdint>

#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"

namespace Util {
namespace Graphic {
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class LinearFrameBufferProvider : public Util::Reflection::Prototype {

public:
    /**
     * Information about a resolution.
     *
     * @var resolutionX Horizontal resolution
     * @var resolutionY Vertical resolution
     * @var colorDepth Color depth in bits
     * @var pith The pitch
     * @var modeNumber Unique number, used to identify the mode. May be ignored if not needed.
     */
    struct ModeInfo {
        uint16_t resolutionX;
        uint16_t resolutionY;
        uint8_t colorDepth;
        uint16_t pitch;
        uint16_t modeNumber;

        bool operator==(const ModeInfo &other) const {
            return resolutionX == other.resolutionX && resolutionY == other.resolutionY &&
                   pitch == other.pitch && colorDepth == other.colorDepth && modeNumber == other.modeNumber;
        }

        bool operator!=(const ModeInfo &other) const {
            return resolutionX != other.resolutionX || resolutionY != other.resolutionY ||
                    pitch != other.pitch || colorDepth != other.colorDepth || modeNumber != other.modeNumber;
        }
    };

    /**
     * Default Constructor.
     */
    LinearFrameBufferProvider() = default;

    /**
     * Copy Constructor.
     */
    LinearFrameBufferProvider(const LinearFrameBufferProvider &other) = delete;

    /**
     * Assignment operator.
     */
    LinearFrameBufferProvider &operator=(const LinearFrameBufferProvider &other) = delete;

    /**
     * Destructor.
     */
    ~LinearFrameBufferProvider() override = default;

    /**
     * Create a linear frame buffer with a given mode info.
     *
     * @param modeInfo Information about the desired frame buffer resolution
     * @param filename The name of the file, representing the created frame buffer in '/device/'
     */
    void initializeLinearFrameBuffer(const ModeInfo &modeInfo, const Util::Memory::String &filename);

    /**
     * Get all available graphics modes.
     *
     * @return An array, containing all available graphics modes
     */
    [[nodiscard]] virtual Util::Data::Array<ModeInfo> getAvailableModes() const = 0;

    /**
     * Search for a graphics mode, that suits the given parameters best.
     *
     * @param resolutionX The desired width
     * @param resolutionY The desired height
     * @param colorDepth The desired color depth
     * @return The found ModeInfo struct
     */
    [[nodiscard]] ModeInfo searchMode(uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth) const;

    /**
     * Overriding function from Prototype.
     */
    [[nodiscard]] Util::Memory::String getClassName() const override = 0;

protected:

    virtual Util::Graphic::LinearFrameBuffer* initializeLinearFrameBuffer(const ModeInfo &modeInfo) = 0;
};

}

#endif
