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

#include "SharedMemory.h"

#include "util/base/Constants.h"
#include "util/base/System.h"
#include "lib/interface.h"

namespace Util::Async {

SharedMemory::SharedMemory(const String &name, const size_t pageCount) : name(name), pageCount(pageCount) {
    auto &memoryManager = System::getAddressSpaceHeader().heapMemoryManager;
    address = memoryManager.allocateMemory(pageCount * PAGESIZE, PAGESIZE);
    Address(address).setRange(0, pageCount * PAGESIZE);
}

SharedMemory::SharedMemory(const size_t processId, const String &name, const size_t pageCount) :
        process(processId), name(name), pageCount(pageCount) {
    auto &memoryManager = System::getAddressSpaceHeader().heapMemoryManager;
    address = memoryManager.allocateMemory(pageCount * PAGESIZE, PAGESIZE);
}

SharedMemory::~SharedMemory() {
    delete static_cast<uint8_t*>(address);
}

Address SharedMemory::getAddress() const {
    return Address(address);
}

bool SharedMemory::publish() const {
    if (process.getId() != Process::getCurrentProcess().getId()) {
        return false;
    }

    return createSharedMemory(name, address, pageCount);
}

bool SharedMemory::map() const {
    if (process.getId() == Process::getCurrentProcess().getId()) {
        return false;
    }

    auto file = Io::File(String::format("/process/%u/shared/%s", process, static_cast<const char*>(name)));
    return file.controlFile(MAP, {reinterpret_cast<size_t>(address)});
}

}
