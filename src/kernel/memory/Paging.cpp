/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "Paging.h"
#include "lib/util/base/Address.h"
#include "device/cpu/Cpu.h"

namespace Kernel {

void Paging::Table::clear() {
    Util::Address(this).setRange(0, sizeof(Paging::Table));
}

Paging::Entry &Paging::Table::operator[](uint32_t index) {
    return entries[index];
}

bool Paging::Table::isEmpty() {
    for (const auto &entry : entries) {
        if (!entry.isUnused()) {
            return false;
        }
    }

    return true;
}

bool Paging::Entry::isUnused() const {
    return address == 0 && flags == 0;
}

void Paging::Entry::set(uint32_t address, uint16_t flags) {
    this->flags = flags;
    this->address = address >> 12;
}

void Paging::Entry::clear() {
    set(0, 0);
}

uint32_t Paging::Entry::getAddress() const {
    return address << 12;
}

uint16_t Paging::Entry::getFlags() const {
    return flags;
}

void Paging::loadDirectory(const Paging::Table &directory) {
    Device::Cpu::writeCr3(&directory);
}

}