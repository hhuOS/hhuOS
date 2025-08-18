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

#include "Pipe.h"

#include "Process.h"
#include "kernel/service/ProcessService.h"

Kernel::Pipe::Pipe(uint32_t readerId) : FilterInputStream(inputStream), FilterOutputStream(outputStream), readerId(readerId) {
    inputStream.connect(outputStream);
}

int16_t Kernel::Pipe::read() {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();
    if (currentProcess.getId() != readerId) {
        return -1;
    }

    return FilterInputStream::read();
}

int16_t Kernel::Pipe::peek() {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();
    if (currentProcess.getId() != readerId) {
        return -1;
    }

    return FilterInputStream::peek();
}

int32_t Kernel::Pipe::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();
    if (currentProcess.getId() != readerId) {
        return -1;
    }

    return FilterInputStream::read(targetBuffer, offset, length);
}

bool Kernel::Pipe::isReadyToRead() {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();
    if (currentProcess.getId() != readerId) {
        return false;
    }

    return FilterInputStream::isReadyToRead();
}

void Kernel::Pipe::write(uint8_t c) {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();

    if (checkWriteAccess(currentProcess.getId())) {
        FilterOutputStream::write(c);
    }
}

void Kernel::Pipe::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();

    if (checkWriteAccess(currentProcess.getId())) {
        FilterOutputStream::write(sourceBuffer, offset, length);
    }
}

void Kernel::Pipe::flush() {
    const auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();

    if (checkWriteAccess(currentProcess.getId())) {
        FilterOutputStream::flush();
    }
}

bool Kernel::Pipe::checkWriteAccess(uint32_t processId) {
    auto atomicWriterId = Util::Async::Atomic(writerId);
    return atomicWriterId.compareAndSet(0, processId) || atomicWriterId.get() == processId;
}
