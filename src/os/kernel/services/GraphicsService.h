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
#include <lib/util/HashMap.h>
#include <kernel/log/Logger.h>

class GraphicsService : public KernelService {

private:

    static Logger &log;

    Util::HashMap<String, LinearFrameBuffer*> lfbMap;
    Util::HashMap<String, TextDriver*> textMap;

    LinearFrameBuffer *currentLfb = nullptr;
    TextDriver *currentTextDriver = nullptr;

public:

    /**
     * Constructor.
     */
    GraphicsService() = default;

    /**
     * Copy-constructor.
     */
    GraphicsService(const GraphicsService &copy) = delete;

    /**
     * Destructor.
     */
    ~GraphicsService() = default;

    /**
     * Register an implementation of the LinearFrameBuffer interface.
     */
    void registerLinearFrameBuffer(LinearFrameBuffer *lfb);

    /**
     * Register an implementation of the TextDriver interface.
     */
    void registerTextDriver(TextDriver *text);

    /**
     * Get the names of all registered LinearFrameBuffers.
     */
    Util::Array<String> getAvailableLinearFrameBuffers();

    /**
     * Get the names of all registered TextDrivers.
     */
    Util::Array<String> getAvailableTextDrivers();

    /**
     * Set the used LinearFrameBuffer to an implementation, that is registered at the GraphicsService.
     *
     * Calling this function will NOT initialize the new LinearFrameBuffer.
     * To initialize it, call getLinearFrameBuffer()->init().
     */
    void setLinearFrameBuffer(const String &name);

    /**
     * Set the used TextDriver to an implementation, that is registered at the GraphicsService.
     *
     * Calling this function will NOT initialize the new TextDriver.
     * To initialize it, call getTextDriver()->init().
     */
    void setTextDriver(const String &name);

    /**
     * Get current lfb.
     */
    LinearFrameBuffer *getLinearFrameBuffer();

    /**
     * Get current text driver.
     */
    TextDriver *getTextDriver();

    static const constexpr char* SERVICE_NAME = "GraphicsService";
};


#endif