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

#ifndef HHUOS_BUFFEREDLINEARFRAMEBUFFER_H
#define HHUOS_BUFFEREDLINEARFRAMEBUFFER_H

#include <stdint.h>

#include "LinearFrameBuffer.h"

namespace Util::Graphic {

class BufferedLinearFrameBuffer : public LinearFrameBuffer {

public:
    /**
     * Constructor.
     *
     * @param lfb The linear frame buffer, that shall be double buffered.
     */
    explicit BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb);

    /**
     * Constructor using a different resolution (must be smaller) than the target lfb.
     * The picture will be centered and scaled to the target resolution.
     */
    BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t resolutionX, uint16_t resolutionY);

    /**
     * Constructor using a scaled down resolution.
     * The picture will be centered and scaled to the target resolution.
     */
    BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, double scaleFactor);

    /**
     * Assignment operator.
     */
    BufferedLinearFrameBuffer& operator=(const BufferedLinearFrameBuffer &other) = delete;

    /**
     * Copy Constructor.
     */
    BufferedLinearFrameBuffer(const BufferedLinearFrameBuffer &copy) = delete;

    /**
     * Destructor.
     */
    ~BufferedLinearFrameBuffer() override = default;

    void flush() const;

private:

    /**
     * Private constructor with all possible parameters to avoid duplicate code.
     */
    BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t resolutionX, uint16_t resolutionY, uint16_t pitch);

    void scalingFlush32() const;

    void scalingFlush24() const;

    void scalingFlush16() const;

    const uint8_t scale;
    const uint16_t offsetX;
    const uint16_t offsetY;

    const LinearFrameBuffer &target;
};

}

#endif