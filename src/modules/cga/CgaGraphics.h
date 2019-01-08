/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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


#ifndef __CgaGraphics_include__
#define __CgaGraphics_include__

#include <cstdint>
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "kernel/Kernel.h"

/**
 * Implementation of LinearFrameBuffer for graphics cards, that are compatible with the CGA-standard.
 */
class CgaGraphics : public LinearFrameBuffer {

private:
    uint32_t videoMemorySize;

    String deviceName;

    Util::Array<LfbResolution> resolutions;

    static const constexpr char *NAME = "CgaGraphics";

private:
    /**
     * Set the CGA-device to a given mode.
     *
     * @param modeNumber The mode
     */
    void setMode(uint16_t modeNumber);

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    bool setResolution(LinearFrameBuffer::LfbResolution resolution) override;

    /**
     * Reallocate the buffer, that is used for double-buffering.
     */
    void reallocBuffer();

public:
    /**
     * Constructor.
     */
    CgaGraphics();

    /**
     * Copy-constructor.
     */
    CgaGraphics(const CgaGraphics &copy) = delete;

    /**
     * Destructor.
     */
    ~CgaGraphics() override = default;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    String getName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    bool isAvailable() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    Util::Array<LinearFrameBuffer::LfbResolution> getLfbResolutions() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    String getDeviceName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    uint32_t getVideoMemorySize() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void drawPixel(uint16_t x, uint16_t y, Color color) override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void readPixel(uint16_t x, uint16_t y, Color &color) override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void clear() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void enableDoubleBuffering() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void disableDoubleBuffering() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    bool isDoubleBuffered() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void show() override;
};

#endif