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

#include "BinaryLoader.h"

#include <stdint.h>

#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/file/elf/File.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "lib/util/base/Panic.h"
#include "lib/util/base/Address.h"
#include "kernel/service/Service.h"
#include "lib/util/base/System.h"
#include "lib/util/base/Constants.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {

BinaryLoader::BinaryLoader(const Util::String &path, const Util::String &command, const Util::Array<Util::String> &arguments) :
        path(path), command(command), arguments(arguments) {}

void BinaryLoader::run() {
    auto file = Util::Io::File(path);
    if (!file.exists()) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "BinaryLoader: File not found!");
    }

    if (!file.isFile()) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "BinaryLoader: Not a file!");
    }

    auto *buffer = new uint8_t[file.getLength()];
    auto binaryStream = Util::Io::FileInputStream(file);
    binaryStream.read(buffer, 0, file.getLength());

    // Buffer is automatically deleted by file destructor
    auto executable = Util::Io::Elf::File(buffer);
    executable.loadProgram();

    // Needed for allocating memory before user space heap
    auto *currentAddress = reinterpret_cast<uint8_t*>(executable.getEndAddress());

    auto &addressSpaceHeader = *reinterpret_cast<Util::System::AddressSpaceHeader*>(Util::USER_SPACE_MEMORY_START_ADDRESS);

    // Copy symbol and string table to user space (needed for stack trace with symbol names)
    auto &symbolTableHeader = executable.getSectionHeader(Util::Io::Elf::SectionHeaderType::SYMTAB);
    auto &stringTableHeader = executable.getSectionHeader(Util::Io::Elf::SectionHeaderType::STRTAB);
    addressSpaceHeader.symbolTableSize = symbolTableHeader.size;

    auto symbolTableAddress = Util::Address(buffer + symbolTableHeader.offset);
    auto stringTableAddress = Util::Address(buffer + stringTableHeader.offset);

    Util::Address(currentAddress).copyRange(symbolTableAddress, symbolTableHeader.size);
    addressSpaceHeader.symbolTable = reinterpret_cast<const Util::Io::Elf::SymbolEntry*>(currentAddress);
    currentAddress += symbolTableHeader.size;

    Util::Address(currentAddress).copyRange(stringTableAddress, stringTableHeader.size);
    addressSpaceHeader.stringTable = reinterpret_cast<const char*>(currentAddress);
    currentAddress += stringTableHeader.size;

    // Copy arguments to user space
    uint32_t argc = arguments.length() + 1;
    char **argv = reinterpret_cast<char**>(executable.getEndAddress() + 1);
    currentAddress += sizeof(char**) * argc;

    for (uint32_t i = 0; i < argc; i++) {
        auto sourceArgument = Util::Address(static_cast<const char*>(i == 0 ? command : arguments[i - 1]));
        auto targetArgument = Util::Address(currentAddress);

        targetArgument.copyString(sourceArgument);
        argv[i] = reinterpret_cast<char*>(currentAddress);
        currentAddress += targetArgument.stringLength() + 1;
    }

    auto &processService = Service::getService<ProcessService>();
    auto &process = processService.getCurrentProcess();
    auto heapAddress = Util::Address(currentAddress + 1).alignUp(Util::PAGESIZE).get();
    auto &userThread = Thread::createMainUserThread(file.getName(), process, (uint32_t) executable.getEntryPoint(), argc, argv, nullptr, heapAddress);

    processService.getCurrentProcess().setMainThread(userThread);
    processService.getScheduler().ready(userThread);
}

}