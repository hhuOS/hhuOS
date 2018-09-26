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

#ifndef __GraphicsService_include__
#define __GraphicsService_include__

#include <kernel/KernelService.h>
#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include <devices/graphics/text/TextDriver.h>
#include <lib/util/RingBuffer.h>
#include <kernel/events/graphics/TextDriverChangedEvent.h>
#include <kernel/events/graphics/LfbDriverChangedEvent.h>

class GraphicsService : public KernelService {

private:
    LinearFrameBuffer *lfb = nullptr;
    TextDriver *text = nullptr;

    Util::RingBuffer<TextDriverChangedEvent> textEventBuffer;
    Util::RingBuffer<LfbDriverChangedEvent> lfbEventBuffer;

public:
    /**
     * Constructor.
     */
    GraphicsService();

    /**
     * Copy-constructor.
     */
    GraphicsService(const GraphicsService &copy) = delete;

    /**
     * Destructor.
     */
    ~GraphicsService() = default;

    /**
     * Get current lfb.
     */
    LinearFrameBuffer *getLinearFrameBuffer();

    /**
     * Set current lfb.
     */
    void setLinearFrameBuffer(LinearFrameBuffer *lfb);

    /**
     * Get current text driver.
     */
    TextDriver *getTextDriver();

    /**
     * Set current text driver.
     */
    void setTextDriver(TextDriver *text);

    static const constexpr char* SERVICE_NAME = "GraphicsService";
};


#endif