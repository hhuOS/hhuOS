/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/base/System.h"
#include "lib/util/async/Process.h"
#include "lib/util/graphic/Terminal.h"
#include "Shell.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/PrintStream.h"

Shell::Shell(const Util::String &path) : startDirectory(path) {}

void Shell::run() {
    if (!Util::Io::File::changeDirectory(startDirectory)) {
        Util::System::error << "Unable to start shell in '" << startDirectory << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
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
    auto currentDirectory = Util::Io::File::getCurrentWorkingDirectory();

    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN << "["
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_WHITE << (currentDirectory.getCanonicalPath().isEmpty() ? "/" : currentDirectory.getName())
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN << "]> "
                      << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Io::PrintStream::flush;

    startPosition = Util::Graphic::Ansi::getCursorPosition();
}

void Shell::readLine() {
    Util::Graphic::Ansi::enableRawMode();
    int16_t input = Util::Graphic::Ansi::readChar();

    while (isRunning) {
        switch (input) {
            case Util::Graphic::Ansi::KEY_UP:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleUpKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::KEY_DOWN:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleDownKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::KEY_RIGHT:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleRightKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::KEY_LEFT:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleLeftKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case 0x08:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleBackspace();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case 0x09:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleTab();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case 0x0a:
                currentLine = currentLine.strip();
                Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                Util::Graphic::Ansi::enableCanonicalMode();
                return;
            case 0x00 ... 0x06:
            case 0x0b ... 0x1f:
                break;
            default:
                currentLine += static_cast<char>(input);
                Util::System::out << static_cast<char>(input) << Util::Io::PrintStream::flush;
        }

        input = Util::Graphic::Ansi::readChar();
    }

    Util::Graphic::Ansi::enableCanonicalMode();
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

Util::String Shell::checkPath(const Util::String &command) const {
    for (const auto &path : Util::String(PATH).split(":")) {
        auto file = Util::Io::File(path);
        auto binaryPath = checkDirectory(command, file);
        if (!binaryPath.isEmpty()) {
            return binaryPath;
        }
    }

    return "";
}

Util::String Shell::checkDirectory(const Util::String &command, Util::Io::File &directory) const {
    if (!directory.exists() || !directory.isDirectory()) {
        return "";
    }

    for (const auto &child : directory.getChildren()) {
        auto file = Util::Io::File(directory.getCanonicalPath() + "/" + child);
        if (file.isFile() && file.getName() == command) {
            return file.getCanonicalPath();
        }

        if (file.isDirectory()) {
            checkDirectory(command, file);
        }
    }

    return "";
}

void Shell::cd(const Util::Array<Util::String> &arguments) {
    if (arguments.length() == 0) {
        return;
    }

    const auto &path = arguments[0];
    auto file = Util::Io::File(path);

    if (!file.exists()) {
        Util::System::out << "cd: '" << path << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    if (file.isFile()) {
        Util::System::out << "cd: '" << path << "' is not a directory!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto result = Util::Io::File::changeDirectory(path);
    if (!result) {
        Util::System::out << "cd: Failed to change directory!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }
}

void Shell::executeBinary(const Util::String &path, const Util::String &command, const Util::Array<Util::String> &arguments, const Util::String &outputPath, bool async) {
    auto binaryFile = Util::Io::File(path);
    auto inputFile = Util::Io::File("/device/terminal");
    auto outputFile = Util::Io::File(outputPath);

    if (!binaryFile.exists()) {
        Util::System::out << "'" << path << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    if (binaryFile.isDirectory()) {
        Util::System::out << "'" << path << "' is a directory!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    if (!outputFile.exists() && !outputFile.create(Util::Io::File::REGULAR)) {
        Util::System::out << "Failed to execute file '" << path << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto process = Util::Async::Process::execute(binaryFile, inputFile, outputFile, outputFile, command, arguments);
    if (!async) {
        process.join();
    }
}

void Shell::handleUpKey() {
    if (history.isEmpty()) {
        return;
    }

    if (historyIndex == history.size()) {
        historyCurrentLine = currentLine;
    }

    while (Util::Graphic::Ansi::getCursorPosition().row > startPosition.row) {
        Util::Graphic::Ansi::clearLine();
        Util::Graphic::Ansi::moveCursorUp(1);
    }

    Util::Graphic::Ansi::setPosition(startPosition);
    Util::Graphic::Ansi::clearLineFromCursor();

    historyIndex = historyIndex == 0 ? 0 : historyIndex - 1;
    auto historyLine = history.get(historyIndex);
    Util::System::out << historyLine << Util::Io::PrintStream::flush;
    currentLine = historyLine;
}

void Shell::handleDownKey() {
    if (history.isEmpty()) {
        return;
    }

    Util::Io::File::control(Util::Io::STANDARD_INPUT, Util::Graphic::Terminal::SET_ANSI_PARSING, {true});
    while (Util::Graphic::Ansi::getCursorPosition().row > startPosition.row) {
        Util::Graphic::Ansi::clearLine();
        Util::Graphic::Ansi::moveCursorUp(1);
    }

    Util::Graphic::Ansi::setPosition(startPosition);
    Util::Graphic::Ansi::clearLineFromCursor();

    Util::String historyLine;
    if (historyIndex >= history.size() - 1 || history.size() == 1) {
        historyIndex = history.size();
        historyLine = historyCurrentLine;
    } else {
        historyIndex = historyIndex == history.size() - 1 ? history.size() - 1 : historyIndex + 1;
        historyLine = history.get(historyIndex);
    }

    Util::System::out << historyLine << Util::Io::PrintStream::flush;
    currentLine = historyLine;
}

void Shell::handleLeftKey() {

}

void Shell::handleRightKey() {

}

void Shell::handleBackspace() {
    if (currentLine.length() > 0) {
        currentLine = currentLine.substring(0, currentLine.length() - 1);

        auto position = Util::Graphic::Ansi::getCursorPosition();
        auto limits = Util::Graphic::Ansi::getCursorLimits();

        if (position.column == 0) {
            position.row = position.row == 0 ? 0 : position.row - 1;
            position.column = limits.column;
            Util::Graphic::Ansi::setPosition(position);
            Util::System::out << ' ' << Util::Io::PrintStream::flush;
            Util::Graphic::Ansi::setPosition(position);
        } else {
            Util::Graphic::Ansi::moveCursorLeft(1);
            Util::System::out << ' ' << Util::Io::PrintStream::flush;
            Util::Graphic::Ansi::moveCursorLeft(1);
        }
    }
}

void Shell::handleTab() {

}
