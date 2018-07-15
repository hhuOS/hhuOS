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

#include "RandomNode.h"
#include "lib/Random.h"

RandomNode::RandomNode() : VirtualNode("random", FsNode::CHAR_FILE) {

}

uint64_t RandomNode::getLength() {
    return 0;
}

uint64_t RandomNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    auto *random = new Random(0xFF);
    
    for(uint32_t i = 0; i < numBytes; i++) {
        buf[i] = (char) random->rand();
    }
    
    delete random;
    return numBytes;
}

uint64_t RandomNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}