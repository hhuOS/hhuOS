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

#ifndef HHUOS_CURRENTRESOLUTIONNODE_H
#define HHUOS_CURRENTRESOLUTIONNODE_H

#include "GraphicsNode.h"

/**
 * Implementation of GraphicsNode, that contains the current resolution.
 * Writing to this node will result in a resolution change.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class CurrentResolutionNode : public GraphicsNode {

public:

    /**
     * Constructor.
     */
    explicit CurrentResolutionNode(GraphicsNode::GraphicsMode mode);

    /**
     * Copy-constructor.
     */
    CurrentResolutionNode(const CurrentResolutionNode &copy) = delete;

    /**
     * Destructor.
     */
    ~CurrentResolutionNode() override = default;

    /**
     * Overriding function from GraphicsNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from GraphicsNode.
     */
    void writeValuesToCache() override;
};

#endif
