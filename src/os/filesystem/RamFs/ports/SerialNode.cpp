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

#include "SerialNode.h"

String SerialNode::generateName(Serial::ComPort port) {
    switch(port) {
        case Serial::COM1 :
            return "serial1";
        case Serial::COM2 :
            return "serial2";
        case Serial::COM3 :
            return "serial3";
        case Serial::COM4 :
            return "serial4";
        default:
            return "serial";
    }
}

SerialNode::SerialNode(Serial *serial) : VirtualNode(generateName(serial->getPortNumber()), FsNode::BLOCK_FILE), serial(serial) {

}

uint64_t SerialNode::getLength() {
    return 0;
}

uint64_t SerialNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    serial->readData(buf, static_cast<uint32_t>(numBytes));

    // Convert carriage returns to '\n'
    for(uint32_t i = 0; i < numBytes; i++) {
        if(buf[i] == 13) {
            buf[i] = '\n';
        }
    }

    return numBytes;
}

uint64_t SerialNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    serial->sendData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}
