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

#include "SerialEvent.h"

SerialEvent::SerialEvent() : Event(TYPE) {

}

SerialEvent::SerialEvent(Serial::ComPort port, char c) : Event(TYPE), port(port), c(c) {

}

char *SerialEvent::getName() {
    return const_cast<char *>("SerialEvent");
}

char SerialEvent::getChar() {
    return c;
}

SerialEvent::SerialEvent(const SerialEvent &other) : Event(other){
    this->c = other.c;
    this->port = other.port;
}

Serial::ComPort SerialEvent::getPortNumber() {
    return port;
}
