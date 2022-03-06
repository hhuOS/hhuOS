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

#include "kernel/system/System.h"
#include "kernel/process/BinaryLoader.h"
#include "lib/util/stream/InputStreamReader.h"
#include "lib/util/stream/PrintWriter.h"
#include "lib/util/stream/FileInputStream.h"
#include "lib/util/stream/FileOutputStream.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/time/Date.h"
#include "Shell.h"

Shell::~Shell() {
    delete reader;
}

void Shell::run() {
    auto currentDirectory = Util::File::getCurrentWorkingDirectory();
    auto inputStream = Util::Stream::FileInputStream("/device/keyboard");
    reader = new Util::Stream::InputStreamReader(inputStream);
    Util::Memory::String line = "";

    Util::System::out << Util::Graphic::Ansi::BRIGHT_GREEN << "["
            << Util::Graphic::Ansi::BRIGHT_WHITE << (currentDirectory.getCanonicalPath().isEmpty() ? "/" : currentDirectory.getName())
            << Util::Graphic::Ansi::BRIGHT_GREEN << "]> "
            << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::flush;

    while(true) {
        char input = reader->read();
        Util::System::out << input << Util::Stream::PrintWriter::flush;

        if (input == '\n') {
            parseInput(line);
            line = "";
            currentDirectory = Util::File::getCurrentWorkingDirectory();

            Util::System::out << Util::Graphic::Ansi::BRIGHT_GREEN << "["
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

    if (command == "cd") {
        cd(arguments);
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

    return Util::File::File(Util::File::getCurrentWorkingDirectory().getCanonicalPath() + "/" + path);
}

void Shell::invalid(const Util::Data::Array<Util::Memory::String> &arguments) {
    Util::System::out << "Invalid command! Use 'help' to see available commands." << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

void Shell::help(const Util::Data::Array<Util::Memory::String> &arguments) {
    Util::System::out << "uptime - Print system uptime" << Util::Stream::PrintWriter::endl
            << "date - Print current date" << Util::Stream::PrintWriter::endl
            << "cat [file]... - Print files consecutively" << Util::Stream::PrintWriter::endl
            << "ls [file]... - Print all files in a directory" << Util::Stream::PrintWriter::endl
            << "ls [file]... - Print filesystem ls" << Util::Stream::PrintWriter::endl
            << "help - Print available commands" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

void Shell::cd(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() == 0) {
        return;
    }

    auto path = arguments[0];
    auto file = getFile(path);

    if (!file.exists()) {
        Util::System::out << "cd: '" << path << "' not found!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    if (file.isFile()) {
        Util::System::out << "cd: '" << path << "' is not a directory!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    auto result = Util::File::changeDirectory(path);
    if (!result) {
        Util::System::out << "cd: Failed to change directory!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }
}

void Shell::executeBinary(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    const auto file = getFile(path);
    if (!file.exists()) {
        Util::System::out << "'" << path << "' not found!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    if (file.isDirectory()) {
        Util::System::out << "'" << path << "' is a directory!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();

    auto &virtualAddressSpace = memoryService.createAddressSpace();
    auto &process = schedulerService.createProcess(virtualAddressSpace, Util::File::getCurrentWorkingDirectory());
    auto &thread = Kernel::Thread::createKernelThread("Loader", new Kernel::BinaryLoader(file.getCanonicalPath(), command, arguments));

    process.ready(thread);
    schedulerService.ready(process);

    while (!process.isFinished());
}

Util::Memory::String Shell::checkPath(const Util::Memory::String &command) {
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
