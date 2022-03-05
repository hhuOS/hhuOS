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

    if (command == "uptime") {
        uptime(arguments);
    } else if (command == "date") {
        date(arguments);
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
            << "tree [file]... - Print filesystem tree" << Util::Stream::PrintWriter::endl
            << "help - Print available commands" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
}

void Shell::uptime(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &timeService = Kernel::System::getService<Kernel::TimeService>();
    Util::System::out << Util::Stream::PrintWriter::dec << timeService.getSystemTime().toSeconds() << " seconds" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

void Shell::date(const Util::Data::Array<Util::Memory::String> &arguments) {
    auto &timeService = Kernel::System::getService<Kernel::TimeService>();
    auto date = timeService.getCurrentDate();
    Util::System::out << Util::Memory::String::format("%u-%02u-%02u %02u:%02u:%02u",
                                           date.getYear(), date.getMonth(), date.getDayOfMonth(),
                                           date.getHours(), date.getMinutes(), date.getSeconds()) << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

void Shell::cat(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() < 1) {
        Util::System::out << "No arguments provided!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return;
    }

    for (const auto &path : arguments) {
        auto file = getFile(path);
        if (!file.exists()) {
            Util::System::out << "cat: '" << path << "' not found!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
            continue;
        }

        if (file.isDirectory()) {
            Util::System::out << "cat: '" << path << "' is a directory!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
            continue;
        }

        auto fileType = file.getType();
        auto fileInputStream = Util::Stream::FileInputStream(file);
        auto fileReader = Util::Stream::InputStreamReader(fileInputStream);
        auto bufferedFileReader = Util::Stream::BufferedReader(fileReader);
        char logChar = bufferedFileReader.read();

        if (fileType == Util::File::REGULAR) {
            while (logChar != -1) {
                Util::System::out << logChar;
                logChar = bufferedFileReader.read();
            }
        } else {
            while (logChar != -1) {
                Util::System::out << logChar << Util::Stream::PrintWriter::flush;
                logChar = bufferedFileReader.read();
            }
        }
    }

    Util::System::out << Util::Stream::PrintWriter::flush;
}

void Shell::cd(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() == 0) {
        return;
    }

    auto path = arguments[0];
    auto file = getFile(path);

    if (!file.exists()) {
        Util::System::out << "cd: '" << path << "' not found!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return;
    }

    if (file.isFile()) {
        Util::System::out << "cd: '" << path << "' is not a directory!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return;
    }

    auto result = Util::File::changeDirectory(path);
    if (!result) {
        Util::System::out << "cd: Failed to change directory!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return;
    }
}

void Shell::ls(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() == 0) {
        lsDirectory(Util::File::getCurrentWorkingDirectory().getCanonicalPath());
    } else {
        for (const auto &path : arguments) {
            lsDirectory(path);
        }
    }
}

void Shell::lsDirectory(const Util::Memory::String &path) {
    const auto file = getFile(path);
    if (!file.exists()) {
        Util::System::out << "ls '" << path << "' not found!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
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

    Util::System::out << string << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
}

void Shell::tree(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() == 0) {
        treeDirectory(Util::File::getCurrentWorkingDirectory().getCanonicalPath(), 0);
    } else {
        for (const auto &path : arguments) {
            treeDirectory(path);
        }
    }
}

void Shell::treeDirectory(const Util::Memory::String &path, uint32_t level) {
    const auto file = getFile(path);
    if (!file.exists()) {
        Util::System::out << "tree '" << path << "' not found!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return;
    }

    auto string = Util::Memory::String("|-");
    for (uint32_t i = 0; i < level; i++) {
        string += "-";
    }

    string += getFileColor(file) + file.getName() + (file.isDirectory() ? "/" : "") + Util::Graphic::Ansi::RESET + " ";
    Util::System::out << string << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;

    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            treeDirectory(file.getCanonicalPath() + "/" + child, level + 1);
        }
    }
}

void Shell::executeBinary(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments) {
    const auto file = getFile(path);
    if (!file.exists()) {
        Util::System::out << "'" << path << "' not found!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
        return;
    }

    if (file.isDirectory()) {
        Util::System::out << "'" << path << "' is a directory!" << Util::Stream::PrintWriter::endl  << Util::Stream::PrintWriter::flush;
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
