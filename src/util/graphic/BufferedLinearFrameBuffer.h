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

#ifndef HHUOS_BUFFEREDLINEARFRAMEBUFFER_H
#define HHUOS_BUFFEREDLINEARFRAMEBUFFER_H

#include "LinearFrameBuffer.h"

namespace Util::Graphic {

class BufferedLinearFrameBuffer : public LinearFrameBuffer {

public:
    /**
     * Constructor.
     *
     * @param address The buffer address
     * @param resolutionX The horizontal resolution
     * @param resolutionY The vertical resolution
     * @param bitsPerPixel The color colorDepth
     * @param pitch The pitch
     */
    BufferedLinearFrameBuffer(void *address, uint16_t resolutionX, uint16_t resolutionY, uint8_t bitsPerPixel, uint16_t pitch);

    /**
     * Assignment operator.
     */
    BufferedLinearFrameBuffer& operator=(const BufferedLinearFrameBuffer &other) = delete;

    /**
     * Copy constructor.
     */
    BufferedLinearFrameBuffer(const BufferedLinearFrameBuffer &copy) = delete;

    /**
     * Destructor.
     */
    ~BufferedLinearFrameBuffer() override;

    [[nodiscard]] uint8_t* getBuffer() const override;

    void flush() const;

private:

    uint8_t *softwareBuffer;

};

}

#endif
