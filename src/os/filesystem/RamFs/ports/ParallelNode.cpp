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

#include "ParallelNode.h"

String ParallelNode::generateName(Parallel::LptPort port) {
    switch(port) {
        case Parallel::LPT1 :
            return "parallel1";
        case Parallel::LPT2 :
            return "parallel2";
        case Parallel::LPT3 :
            return "parallel3";
        default:
            return "parallel";
    }
}

ParallelNode::ParallelNode(Parallel *parallel) : VirtualNode(generateName(parallel->getPortNumber()), FsNode::BLOCK_FILE), parallel(parallel) {

}

uint64_t ParallelNode::getLength() {
    return 0;
}

uint64_t ParallelNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    parallel->readData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}

uint64_t ParallelNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    parallel->sendData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}
