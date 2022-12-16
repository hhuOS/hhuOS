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

#include "lib/util/memory/Address.h"
#include "LinearFrameBufferNode.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

namespace Device::Graphic {

LinearFrameBufferNode::LinearFrameBufferNode(const Util::Memory::String &name, Util::Graphic::LinearFrameBuffer *lfb) :
        Filesystem::Memory::StringNode(name),
        addressBuffer(Util::Memory::String::format("%u", lfb->getBuffer().get())),
        resolutionBuffer(Util::Memory::String::format("%ux%u@%u", lfb->getResolutionX(), lfb->getResolutionY(), lfb->getColorDepth())),
        pitchBuffer(Util::Memory::String::format("%u", lfb->getPitch())) {}

LinearFrameBufferNode::~LinearFrameBufferNode() {
    delete lfb;
}

Util::Memory::String LinearFrameBufferNode::getString() {
    return addressBuffer + "\n" + resolutionBuffer + "\n" + pitchBuffer + "\n";
}

}