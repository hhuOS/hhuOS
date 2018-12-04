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

#include "Prototype.h"

Util::HashMap<String, Prototype*> Prototype::prototypeTable;

Prototype *Prototype::createInstance(String type) {
    String key = type.toLowerCase();

    if(prototypeTable.containsKey(key)) {
        return prototypeTable.get(type)->clone();
    }

    const char* errorMessage = (const char*) String::format("No prototype registered for '%s'!", (const char*) type);

    Cpu::throwException(Cpu::Exception::UNKNOWN_TYPE, errorMessage);

    return nullptr;
}

void Prototype::registerPrototype(Prototype *driver) {
    String key = driver->getName().toLowerCase();

    prototypeTable.put(key, driver);
}

void Prototype::deregisterPrototype(String type) {
    if(prototypeTable.containsKey(type)) {
        prototypeTable.remove(type);
    }
}