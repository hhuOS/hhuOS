/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/stream/BufferedOutputStream.h"
#include "lib/util/stream/InputStreamReader.h"
#include "lib/util/stream/PrintWriter.h"
#include "kernel/system/System.h"
#include "kernel/service/TimeService.h"
#include "kernel/process/BinaryLoader.h"
#include "lib/util/stream/FileInputStream.h"
#include "lib/util/stream/FileOutputStream.h"
#include "lib/util/stream/BufferedReader.h"
#include "lib/util/graphic/Ansi.h"
#include "Shell.h"

Shell::~Shell() {
    delete inputStream;
    delete outputStream;
}

void Shell::run() {
    inputStream = new Util::Stream::FileInputStream("/device/keyboard");
    outputStream = new Util::Stream::FileOutputStream("/device/terminal");

    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto reader = Util::Stream::InputStreamReader(*inputStream);
    Util::Memory::String line = "";

    writer << Util::Graphic::Ansi::BRIGHT_GREEN << "["
            << Util::Graphic::Ansi::BRIGHT_WHITE << (currentDirectory.getCanonicalPath().isEmpty() ? "/" : currentDirectory.getName())
            << Util::Graphic::Ansi::BRIGHT_GREEN << "]> "
            << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::flush;

    while(true) {
        char input = reader.read();
        writer << input << Util::Stream::PrintWriter::flush;

        if (input == '\n') {
            parseInput(line);
            line = "";
            writer << Util::Graphic::Ansi::BRIGHT_GREEN << "["
                    << Util::Graphic::Ansi::BRIGHT_WHITE << (currentDirectory.getCanonicalPath().isEmpty() ? "/" : currentDirectory.getName())
                    << Util::Graphic::Ansi::BRIGHT_GREEN << "]> "
                    << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::flush;
        } else if (input == '\b') {
            line = line.substring(0, line.length() - 1);
        } else {
            line += input;
        }
    }
}

void Shell::parseInput(const Util::Memory::String &input) {
    const auto command = input.substring(0, input.indexOf(" "));
    const auto rest = input.substring(input.indexOf(" "), input.length());
    const auto arguments = rest.split(" ");

    if (command == "uptime") {
        uptime(arguments);
    } else if (command == "date") {
        date(arguments);
    } else if (command == "mem") {
        mem(arguments);
    } else if (command == "cat") {
        cat(arguments);
    } else if (command == "cd") {
        cd(arguments);
    }  else if (command == "ls") {
        ls(arguments);
    } else if (command == "tree") {
        tree(arguments);
    } else if (command == "help") {
        help(arguments);
    } else if (!command.isEmpty()) {
        auto binaryPath = checkPath(command);
        if (binaryPath.isEmpty()) {
            executeBinary(command, command, arguments);
        } else {
            executeBinary(binaryPath, command, arguments);
        }
    }
}

Util::File::File Shell::getFile(const Util::Memory::String &path) {
    if (path[0] == '/') {
        return Util::File::File(path);
    }

    return Util::File::File(currentDirectory.getCanonicalPath() + "/" + path);
}

void Shell::invalid(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    writer << "Invalid command! Use 'help' to see available commands." << Util::Stream::PrintWriter::endl;
}

void Shell::help(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    writer << "uptime - Print system uptime" << Util::Stream::PrintWriter::endl
            << "date - Print current date" << Util::Stream::PrintWriter::endl
            << "cat [file]... - Print files consecutively" << Util::Stream::PrintWriter::endl
            << "ls [file]... - Print all files in a directory" << Util::Stream::PrintWriter::endl
            << "tree [file]... - Print filesystem tree" << Util::Stream::PrintWriter::endl
            << "help - Print available commands" << Util::Stream::PrintWriter::endl;
}

void Shell::uptime(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto &timeService = Kernel::System::getService<Kernel::TimeService>();

    writer << Util::Stream::PrintWriter::dec << timeService.getSystemTime().toSeconds() << " seconds" << Util::Stream::PrintWriter::endl;
}

void Shell::date(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto &timeService = Kernel::System::getService<Kernel::TimeService>();
    auto date = timeService.getCurrentDate();

    writer << Util::Memory::String::format("%u-%02u-%02u %02u:%02u:%02u",
                                           date.getYear(), date.getMonth(), date.getDayOfMonth(),
                                           date.getHours(), date.getMinutes(), date.getSeconds()) << Util::Stream::PrintWriter::endl;
}

void Shell::mem(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto memoryStatus = Kernel::System::getService<Kernel::MemoryService>().getMemoryStatus();

    writer << "Physical:      " << formatMemory(memoryStatus.freePhysicalMemory) << " / " << formatMemory(memoryStatus.totalPhysicalMemory)
           << Util::Stream::PrintWriter::endl
           << "Lower:         " << formatMemory(memoryStatus.freeLowerMemory) << " / " << formatMemory(memoryStatus.totalLowerMemory)
           << Util::Stream::PrintWriter::endl
           << "Kernel:        " << formatMemory(memoryStatus.freeKernelHeapMemory) << " / " << formatMemory(memoryStatus.totalKernelHeapMemory)
           << Util::Stream::PrintWriter::endl
           << "Paging Area:   " << formatMemory(memoryStatus.freePagingAreaMemory) << " / " << formatMemory(memoryStatus.totalPagingAreaMemory)
           << Util::Stream::PrintWriter::endl;
}

void Shell::cat(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto reader = Util::Stream::InputStreamReader(*inputStream);

    if (arguments.length() < 1) {
        writer << "No arguments provided!" << Util::Stream::PrintWriter::endl;
        return;
    }

    for (const auto &path : arguments) {
        auto file = getFile(path);
        if (!file.exists()) {
            writer << "cat: '" << path << "' not found!" << Util::Stream::PrintWriter::endl;
            continue;
        }

        if (file.isDirectory()) {
            writer << "cat: '" << path << "' is a directory!" << Util::Stream::PrintWriter::endl;
            continue;
        }

        auto fileType = file.getType();
        auto fileInputStream = Util::Stream::FileInputStream(file);
        auto fileReader = Util::Stream::InputStreamReader(fileInputStream);
        auto bufferedFileReader = Util::Stream::BufferedReader(fileReader);
        char logChar = bufferedFileReader.read();

        if (fileType == Util::File::REGULAR) {
            while (logChar != -1) {
                writer << logChar;
                logChar = bufferedFileReader.read();
            }
        } else {
            while (logChar != -1) {
                writer << logChar << Util::Stream::PrintWriter::flush;
                logChar = bufferedFileReader.read();
            }
        }
    }

    writer << Util::Stream::PrintWriter::flush;
}

void Shell::cd(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    if (arguments.length() == 0) {
        return;
    }

    auto path = arguments[0];
    auto file = getFile(path);

    if (!file.exists()) {
        writer << "cd '" << path << "' not found!" << Util::Stream::PrintWriter::endl;
        return;
    }

    if (file.isFile()) {
        writer << "cd '" << path << "' is not a directory!" << Util::Stream::PrintWriter::endl;
        return;
    }

    currentDirectory = file;
}

void Shell::ls(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() == 0) {
        lsDirectory(currentDirectory.getCanonicalPath());
    } else {
        for (const auto &path : arguments) {
            lsDirectory(path);
        }
    }
}

void Shell::lsDirectory(const Util::Memory::String &path) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    const auto file = getFile(path);
    if (!file.exists()) {
        writer << "ls '" << path << "' not found!" << Util::Stream::PrintWriter::endl;
        return;
    }

    auto string = Util::Memory::String();
    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            const auto currentFile = Util::File::File(file.getCanonicalPath() + "/" + child);
            string += getFileColor(currentFile) + currentFile.getName() + (currentFile.isDirectory() ? "/" : "") + Util::Graphic::Ansi::RESET + " ";
        }

        string = string.substring(0, string.length() - 1);
    } else {
        string += Util::Graphic::Ansi::BRIGHT_YELLOW + file.getName() + Util::Graphic::Ansi::RESET;
    }

    writer << string << Util::Stream::PrintWriter::endl;
}

void Shell::tree(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    if (arguments.length() == 0) {
        treeDirectory(currentDirectory.getCanonicalPath(), 0);
    } else {
        for (const auto &path : arguments) {
            treeDirectory(path);
        }
    }
}

void Shell::treeDirectory(const Util::Memory::String &path, uint32_t level) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    const auto file = getFile(path);
    if (!file.exists()) {
        writer << "tree '" << path << "' not found!" << Util::Stream::PrintWriter::endl;
        return;
    }

    auto string = Util::Memory::String("|-");
    for (uint32_t i = 0; i < level; i++) {
        string += "-";
    }

    string += getFileColor(file) + file.getName() + (file.isDirectory() ? "/" : "") + Util::Graphic::Ansi::RESET + " ";
    writer << string << Util::Stream::PrintWriter::endl;

    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            treeDirectory(file.getCanonicalPath() + "/" + child, level + 1);
        }
    }
}

void Shell::executeBinary(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    const auto file = getFile(path);
    if (!file.exists()) {
        writer << "'" << path << "' not found!" << Util::Stream::PrintWriter::endl;
        return;
    }

    if (file.isDirectory()) {
        writer << "'" << path << "' is a directory!" << Util::Stream::PrintWriter::endl;
        return;
    }

    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = schedulerService.createProcess(virtualAddressSpace);
    auto &thread = Kernel::Thread::createKernelThread("Loader", new Kernel::BinaryLoader(file.getCanonicalPath(), command, arguments));

    process.ready(thread);
    schedulerService.ready(process);

    while (!process.isFinished());
}

const char* Shell::getFileColor(const Util::File::File &file) {
    switch (file.getType()) {
        case Util::File::DIRECTORY:
            return Util::Graphic::Ansi::BRIGHT_BLUE;
        case Util::File::REGULAR:
            return Util::Graphic::Ansi::WHITE;
        case Util::File::CHARACTER:
            return Util::Graphic::Ansi::BRIGHT_YELLOW;
    }

    return Util::Graphic::Ansi::WHITE;
}

Util::Memory::String Shell::formatMemory(uint32_t value) {
    uint32_t result = value / 1024 / 1024;
    uint32_t rest = value - (result * 1024 * 1024);
    uint32_t comma = (rest * 1000) / 1024 / 1024;

    return Util::Memory::String::format("%u.%u MiB", result, comma);
}

Util::Memory::String  Shell::checkPath(const Util::Memory::String &command) {
    for (const auto &path : Util::Memory::String(PATH).split(":")) {
        auto binaryPath = checkDirectory(command, Util::File::File(path));
        if (!binaryPath.isEmpty()) {
            return binaryPath;
        }
    }

    return "";
}

Util::Memory::String Shell::checkDirectory(const Util::Memory::String &command, const Util::File::File &directory) {
    if (!directory.exists() || !directory.isDirectory()) {
        return false;
    }

    for (const auto &child : directory.getChildren()) {
        auto file = Util::File::File(directory.getCanonicalPath() + "/" + child);
        if (file.isFile() && file.getName() == command) {
            return file.getCanonicalPath();
        }

        if (file.isDirectory()) {
            checkDirectory(command, file);
        }
    }

    return "";
}
