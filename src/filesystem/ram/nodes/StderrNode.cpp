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

#include "StderrNode.h"
#include "kernel/core/Kernel.h"

StderrNode::StderrNode() : VirtualNode("stderr", FsNode::CHAR_FILE) {
    graphicsService = Kernel::getService<GraphicsService>();
    kernelStreamService = Kernel::getService<KernelStreamService>();
}

uint64_t StderrNode::getLength() {
    return 0;
}

uint64_t StderrNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

uint64_t StderrNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    OutputStream *stderr = kernelStreamService->getStderr();

    if(stderr == nullptr) {
        return 0;
    }

    stderr->writeBytes(buf, numBytes);
    stderr->flush();

    return numBytes;
}