/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include "BinaryLoader.h"

#include <cstdint>

#include "lib/util/file/File.h"
#include "lib/util/stream/FileInputStream.h"
#include "lib/util/file/elf/File.h"
#include "kernel/system/System.h"
#include "kernel/paging/Paging.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/Exception.h"
#include "lib/util/memory/Address.h"

namespace Kernel {

BinaryLoader::BinaryLoader(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) :
        path(path), command(command), arguments(arguments) {}

void BinaryLoader::run() {
    auto file = Util::File::File(path);
    if (!file.exists()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "BinaryLoader: File not found!");
    }

    if (!file.isFile()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "BinaryLoader: Not a file!");
    }

    auto *buffer = new uint8_t[file.getLength()];
    auto binaryStream = Util::Stream::FileInputStream(file);
    binaryStream.read(buffer, 0, file.getLength());

    // buffer is automatically deleted by file destructor
    auto executable = Util::File::Elf::File(buffer);
    executable.loadProgram();

    uint32_t argc = arguments.length() + 1;
    char **argv = reinterpret_cast<char**>(executable.getEndAddress() + 1);
    auto currentAddress = reinterpret_cast<uint32_t>(argv) + sizeof(char**) * argc;

    for (uint32_t i = 0; i < argc; i++) {
        auto sourceArgument = Util::Memory::Address<uint32_t>(static_cast<char*>(i == 0 ? command : arguments[i - 1]));
        auto targetArgument = Util::Memory::Address<uint32_t>(currentAddress);

        targetArgument.copyString(sourceArgument);
        argv[i] = reinterpret_cast<char*>(currentAddress);
        currentAddress += targetArgument.stringLength() + 1;
    }

    auto &processService = System::getService<ProcessService>();
    auto &schedulerService = System::getService<SchedulerService>();
    auto &process = processService.getCurrentProcess();
    auto heapAddress = Util::Memory::Address(currentAddress + 1).alignUp(Kernel::Paging::PAGESIZE).get();
    auto &userThread = Thread::createMainUserThread(file.getName(), process, (uint32_t) executable.getEntryPoint(), argc, argv, nullptr, heapAddress);

    processService.getCurrentProcess().setMainThread(userThread);
    schedulerService.ready(userThread);
}

}