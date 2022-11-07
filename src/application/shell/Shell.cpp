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

#include "lib/util/graphic/Ansi.h"
#include "lib/util/system/System.h"
#include "lib/util/async/Process.h"
#include "lib/util/graphic/Terminal.h"
#include "Shell.h"

Shell::Shell(const Util::Memory::String &path) : startDirectory(path) {}

void Shell::run() {
    if (!Util::File::changeDirectory(startDirectory)) {
        Util::System::error << "Unable to start shell in '" << startDirectory << "'!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    beginCommandLine();

    while (isRunning) {
        readLine();
        parseInput();
        if (isRunning) {
            beginCommandLine();
        }
    }
}

void Shell::beginCommandLine() {
    currentLine = "";
    auto currentDirectory = Util::File::getCurrentWorkingDirectory();

    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN << "["
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_WHITE << (currentDirectory.getCanonicalPath().isEmpty() ? "/" : currentDirectory.getName())
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN << "]> "
                      << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Stream::PrintWriter::flush;

    startPosition = Util::Graphic::Ansi::getCursorPosition();
}

void Shell::readLine() {
    Util::Graphic::Ansi::disableAnsiParsing();
    Util::Graphic::Ansi::disableLineAggregation();
    Util::Graphic::Ansi::disableEcho();

    char input = Util::System::in.read();

    while (isRunning && input != '\n') {
        if (input == Util::Graphic::Ansi::ESCAPE_SEQUENCE_START) {
            isEscapeActive = true;
        }

        if (isEscapeActive) {
            currentEscapeSequence += input;

            if (escapeEndCodes.contains(input)) {
                switch (input) {
                    case 'A':
                        handleUpKey();
                        break;
                    case 'B':
                        handleDownKey();
                        break;
                    case 'C':
                        handleRightKey();
                        break;
                    case 'D':
                        handleLeftKey();
                        break;
                    default:
                        Util::Graphic::Ansi::enableAnsiParsing();
                        Util::System::out << currentEscapeSequence << Util::Stream::PrintWriter::flush;
                        Util::Graphic::Ansi::disableAnsiParsing();
                }

                isEscapeActive = false;
                currentEscapeSequence = "";
            }
        } else if (input == '\b') {
            if (currentLine.length() > 0) {
                currentLine = currentLine.substring(0, currentLine.length() - 1);
                Util::Graphic::Ansi::enableAnsiParsing();

                auto position = Util::Graphic::Ansi::getCursorPosition();
                auto limits = Util::Graphic::Ansi::getCursorLimits();

                if (position.column == 0) {
                    position.row = position.row == 0 ? 0 : position.row - 1;
                    position.column = limits.column;
                    Util::Graphic::Ansi::setPosition(position);
                    Util::System::out << ' ' << Util::Stream::PrintWriter::flush;
                    Util::Graphic::Ansi::setPosition(position);
                } else {
                    Util::Graphic::Ansi::moveCursorLeft(1);
                    Util::System::out << ' ' << Util::Stream::PrintWriter::flush;
                    Util::Graphic::Ansi::moveCursorLeft(1);
                }

                Util::Graphic::Ansi::disableAnsiParsing();
            }
        } else if (input >= 0x20) {
            currentLine += input;
            Util::System::out << input << Util::Stream::PrintWriter::flush;
        }

        input = Util::System::in.read();
    }

    Util::Graphic::Ansi::enableAnsiParsing();
    Util::Graphic::Ansi::enableLineAggregation();
    Util::Graphic::Ansi::enableEcho();

    currentLine = currentLine.strip();
    Util::System::out << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

void Shell::parseInput() {
    const auto async = currentLine.endsWith("&");
    const auto pipeSplit = currentLine.substring(0, async ? currentLine.length() - 1 : currentLine.length()).split(">");
    if (pipeSplit.length() == 0) {
        return;
    }

    const auto command = pipeSplit[0].substring(0, currentLine.indexOf(" "));
    const auto rest = pipeSplit[0].substring(currentLine.indexOf(" "), currentLine.length());
    auto arguments = rest.split(" ");

    const auto targetFile = pipeSplit.length() == 1 ? "/device/terminal" : pipeSplit[1].split(" ")[0];

    if (command == "cd") {
        cd(arguments);
    } else if (command == "exit") {
        isRunning = false;
    } else if (!command.isEmpty()) {
        auto binaryPath = checkPath(command);
        if (binaryPath.isEmpty()) {
            executeBinary(command, command, arguments, targetFile, async);
        } else {
            executeBinary(binaryPath, command, arguments, targetFile, async);
        }
    }

    if (!currentLine.isEmpty() && (history.isEmpty() || currentLine != history.get(history.size() - 1))) {
        history.add(currentLine);
    }

    historyIndex = history.size();
}

Util::Memory::String Shell::checkPath(const Util::Memory::String &command) const {
    for (const auto &path : Util::Memory::String(PATH).split(":")) {
        auto file = Util::File::File(path);
        auto binaryPath = checkDirectory(command, file);
        if (!binaryPath.isEmpty()) {
            return binaryPath;
        }
    }

    return "";
}

Util::Memory::String Shell::checkDirectory(const Util::Memory::String &command, Util::File::File &directory) const {
    if (!directory.exists() || !directory.isDirectory()) {
        return "";
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

void Shell::cd(const Util::Data::Array<Util::Memory::String> &arguments) {
    if (arguments.length() == 0) {
        return;
    }

    const auto &path = arguments[0];
    auto file = Util::File::File(path);

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

void Shell::executeBinary(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments, const Util::Memory::String &outputPath, bool async) {
    auto binaryFile = Util::File::File(path);
    auto inputFile = Util::File::File("/device/terminal");
    auto outputFile = Util::File::File(outputPath);

    if (!binaryFile.exists()) {
        Util::System::out << "'" << path << "' not found!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    if (binaryFile.isDirectory()) {
        Util::System::out << "'" << path << "' is a directory!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    if (!outputFile.exists() && !outputFile.create(Util::File::REGULAR)) {
        Util::System::out << "Failed to execute file '" << path << "'!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return;
    }

    Util::Graphic::Ansi::enableAnsiParsing();
    Util::File::controlFile(Util::File::STANDARD_INPUT, Util::Graphic::Terminal::SET_LINE_AGGREGATION, {true});
    Util::File::controlFile(Util::File::STANDARD_INPUT, Util::Graphic::Terminal::SET_ECHO, {true});

    auto process = Util::Async::Process::execute(binaryFile, inputFile, outputFile, outputFile, command, arguments);
    if (!async) {
        process.join();
    }
}

void Shell::handleUpKey() {
    if (history.isEmpty()) {
        return;
    }

    Util::Graphic::Ansi::enableAnsiParsing();
    while (Util::Graphic::Ansi::getCursorPosition().row > startPosition.row) {
        Util::Graphic::Ansi::clearLine();
        Util::Graphic::Ansi::moveCursorUp(1);
    }

    Util::Graphic::Ansi::setPosition(startPosition);
    Util::Graphic::Ansi::clearLineFromCursor();
    Util::Graphic::Ansi::disableAnsiParsing();

    historyIndex = historyIndex == 0 ? 0 : historyIndex - 1;
    auto historyLine = history.get(historyIndex);
    Util::System::out << historyLine << Util::Stream::PrintWriter::flush;
    currentLine = historyLine;
}

void Shell::handleDownKey() {
    if (history.isEmpty()) {
        return;
    }

    Util::File::controlFile(Util::File::STANDARD_INPUT, Util::Graphic::Terminal::SET_ANSI_PARSING,{true});
    while (Util::Graphic::Ansi::getCursorPosition().row > startPosition.row) {
        Util::Graphic::Ansi::clearLine();
        Util::Graphic::Ansi::moveCursorUp(1);
    }

    Util::Graphic::Ansi::setPosition(startPosition);
    Util::Graphic::Ansi::clearLineFromCursor();
    Util::Graphic::Ansi::disableAnsiParsing();

    if (historyIndex >= history.size() - 1 || history.size() == 1) {
        historyIndex = history.size();
        currentLine = "";
        return;
    }

    historyIndex = historyIndex == history.size() - 1 ? history.size() - 1 : historyIndex + 1;
    auto historyLine = history.get(historyIndex);
    Util::System::out << historyLine << Util::Stream::PrintWriter::flush;
    currentLine = historyLine;
}

void Shell::handleLeftKey() {

}

void Shell::handleRightKey() {

}
