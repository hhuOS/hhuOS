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

#ifndef HHUOS_GRAPHICSNODE_H
#define HHUOS_GRAPHICSNODE_H

#include <lib/String.h>
#include <filesystem/FsNode.h>
#include <filesystem/RamFs/VirtualNode.h>
#include <kernel/events/Receiver.h>
#include <kernel/services/GraphicsService.h>

class GraphicsNode : public VirtualNode, Receiver {

public:

    /**
     * Possible graphics modes.
     *
     * @param mode TEXT: Use the current TextDriver.
     *             LINEAR_FRAME_BUFFER: Use the current LinearFrameBuffer.
     */
    enum GraphicsMode {
        TEXT = 0x00,
        LINEAR_FRAME_BUFFER = 0x01
    };

protected:

    String cache;

    GraphicsMode mode;

    GraphicsService *graphicsService = nullptr;

public:

    /**
     * Constructor
     */
    GraphicsNode(String name, GraphicsMode mode);

    /**
     * Copy-constructor.
     */
    GraphicsNode(const GraphicsNode &copy) = delete;

    /**
     * Destructor.
     */
    ~GraphicsNode() override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

    void onEvent(const Event &event) override;

    virtual void writeValuesToCache() = 0;
};

#endif
