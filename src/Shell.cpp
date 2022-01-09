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

    writer << "> " << Util::Stream::PrintWriter::flush;

    while(true) {
        char input = reader.read();
        writer << input << Util::Stream::PrintWriter::flush;

        if (input == '\n') {
            if (line == "tree") {
                listDirectory("/");
            } else if (line == "log") {
                printLog();
            } else if (line == "uptime") {
                auto &timeService = Kernel::System::getService<Kernel::TimeService>();
                writer << Util::Stream::PrintWriter::dec << timeService.getSystemTime().toSeconds() << " seconds" << Util::Stream::PrintWriter::endl;
            } else if (line == "date") {
                auto &timeService = Kernel::System::getService<Kernel::TimeService>();
                auto date = timeService.getCurrentDate();
                writer << Util::Memory::String::format("%u-%02u-%02u %02u:%02u:%02u",
                                                       date.getYear(), date.getMonth(), date.getDayOfMonth(),
                                                       date.getHours(), date.getMinutes(), date.getSeconds()) << Util::Stream::PrintWriter::endl;
            } else if (line == "help") {
                writer << "tree - Print filesystem tree" << Util::Stream::PrintWriter::endl
                       << "log - Print kernel log" << Util::Stream::PrintWriter::endl
                       << "uptime - Print system uptime" << Util::Stream::PrintWriter::endl
                       << "date - Print current date" << Util::Stream::PrintWriter::endl
                       << "help - Print available commands" << Util::Stream::PrintWriter::endl;
            } else {
                writer << "Invalid command! Use 'help' to see available commands." << Util::Stream::PrintWriter::endl;
            }

            line = "";
            writer << "> " << Util::Stream::PrintWriter::flush;
        } else if (input == '\b') {
            line = line.substring(0, line.length() - 1);
        } else if (Util::Memory::String::isAlpha(input)) {
            line += input;
        }
    }
}

void Shell::printLog() {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto reader = Util::Stream::InputStreamReader(*inputStream);

    auto logFile = Util::File::File("/device/log");
    if (!logFile.exists()) {
        writer << "Logfile '/device/log' does not exist!" << Util::Stream::PrintWriter::endl;
    }

    auto logStream = Util::Stream::FileInputStream(logFile);
    auto logReader = Util::Stream::InputStreamReader(logStream);
    auto bufferedLogReader = Util::Stream::BufferedReader(logReader);

    char logChar = bufferedLogReader.read();
    while (logChar != -1) {
        writer << logChar;
        logChar = bufferedLogReader.read();
    }

    writer << Util::Stream::PrintWriter::flush;
}

void Shell::listDirectory(const Util::Memory::String &path, uint32_t level) {
    auto writer = Util::Stream::PrintWriter(*outputStream);

    const auto file = Util::File::File(path);
    if (!file.exists()) {
        return;
    }

    auto string = Util::Memory::String("|-");
    for (uint32_t i = 0; i < level; i++) {
        string += "-";
    }

    if (file.isDirectory()) {
        string += Util::Graphic::Ansi::BRIGHT_GREEN + file.getName() + "/" + Util::Graphic::Ansi::RESET;
    } else {
        string += Util::Graphic::Ansi::BRIGHT_YELLOW + file.getName() + Util::Graphic::Ansi::RESET;
    }

    writer << string << Util::Stream::PrintWriter::endl;

    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            listDirectory(path + "/" + child, level + 1);
        }
    }
}
