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

#include "StdoutNode.h"
#include "kernel/core/Kernel.h"

StdoutNode::StdoutNode() : VirtualNode("stdout", FsNode::CHAR_FILE) {
    kernelStreamService = Kernel::getService<KernelStreamService>();
}

uint64_t StdoutNode::getLength() {
    return 0;
}

uint64_t StdoutNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    return numBytes;
}

uint64_t StdoutNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    OutputStream *stdout = kernelStreamService->getStdout();

    if(stdout == nullptr) {
        return 0;
    }
    
    stdout->writeBytes(buf, numBytes);
    stdout->flush();

    return numBytes;
}