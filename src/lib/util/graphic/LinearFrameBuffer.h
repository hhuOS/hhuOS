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

#ifndef __LinearFrameBuffer_include__
#define __LinearFrameBuffer_include__

#include <stdint.h>

#include "Color.h"
#include "lib/util/base/Address.h"

namespace Util {

namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Graphic {

/**
 * Wraps a chunk of memory, that can be used as a linear frame buffer.
 */
class LinearFrameBuffer {

public:

    enum Request {
        SET_RESOLUTION
    };

    /**
     * Constructor.
     *
     * @param physicalAddress The buffer address
     * @param resolutionX The horizontal resolution
     * @param resolutionY The vertical resolution
     * @param colorDepth The color colorDepth
     * @param pitch The pitch
     */
    LinearFrameBuffer(uint32_t physicalAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch);

    /**
     * Constructor.
     *
     * @param physicalAddress The buffer address
     * @param resolutionX The horizontal resolution
     * @param resolutionY The vertical resolution
     * @param colorDepth The color colorDepth
     * @param pitch The pitch
     */
    LinearFrameBuffer(void *virtualAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch);

    LinearFrameBuffer(Util::Address<uint32_t> *address, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch);

    explicit LinearFrameBuffer(Io::File &file);

    /**
     * Assignment operator.
     */
     LinearFrameBuffer& operator=(const LinearFrameBuffer &other) = delete;

    /**
     * Copy Constructor.
     */
    LinearFrameBuffer(const LinearFrameBuffer &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~LinearFrameBuffer();

    /**
     * Get the horizontal resolution.
     *
     * @return The horizontal resolution
     */
    [[nodiscard]] uint16_t getResolutionX() const;

    /**
     * Get the vertical resolution.
     *
     * @return The vertical resolution
     */
    [[nodiscard]] uint16_t getResolutionY() const;

    /**
     * Get the color colorDepth.
     *
     * @return The color colorDepth
     */
    [[nodiscard]] uint8_t getColorDepth() const;

    /**
     * Get the buffer's pitch.
     *
     * @return The pitch
     */
    [[nodiscard]] uint16_t getPitch() const;

    /**
     * Get the amount of bytes per pixel.
     *
     * @return The amount of bytes per pixel
     */
    [[nodiscard]] uint8_t getBytesPerPixel() const;

    /**
     * Get the buffer address.
     *
     * @return The buffer address
     */
    [[nodiscard]] const Address<uint32_t>& getBuffer() const;

    /**
     * Read the color of a pixel at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param color A reference to the variable, that the pixel's color will be written to
     */
    [[nodiscard]] Color readPixel(uint16_t x, uint16_t y) const;

    [[nodiscard]] bool isCompatibleWith(const LinearFrameBuffer &other) const;

    void clear() const;

private:

    static Address<uint32_t> mapBuffer(void *physicalAddress, uint16_t resolutionY, uint16_t pitch);

    Address<uint32_t> buffer;

    uint16_t resolutionX = 0;
    uint16_t resolutionY = 0;
    uint8_t colorDepth = 0;
    uint16_t pitch = 0;

};

}

#endif
