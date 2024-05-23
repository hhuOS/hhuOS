/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

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
            case 0x0a: {
                Util::Graphic::Ansi::enableCanonicalMode();

                auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
                auto startPosition = getStartPosition();
                uint16_t column = startPosition.column + (currentLine.length() % (cursorLimits.column + 1));
                uint16_t row = startPosition.row + ((currentLine.length() + startPosition.column) / (cursorLimits.column + 1));
                Util::Graphic::Ansi::setPosition({column, row});
                Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

                currentLine = currentLine.strip();
                return;
            }
            case 0x00 ... 0x06:
            case 0x0b ... 0x1f:
                break;
            default:
                Util::Graphic::Ansi::enableCanonicalMode();
                auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
                auto cursorPos = Util::Graphic::Ansi::getCursorPosition();
                auto scrolledLines = getScrolledLines(); // Amount of lines the screen has been scrolled to display the current line
                auto startPosition = getStartPosition();
                auto preCursor = currentLine.substring(0, (cursorPos.row - startPosition.row) * (cursorLimits.column + 1) + cursorPos.column - startPosition.column);
                auto afterCursor = currentLine.substring((cursorPos.row - startPosition.row) * (cursorLimits.column + 1) + cursorPos.column - startPosition.column, currentLine.length());
                currentLine = preCursor + static_cast<char>(input) + afterCursor;

                Util::System::out << static_cast<char>(input) << afterCursor << Util::Io::PrintStream::flush;

                if (getScrolledLines() > scrolledLines) { // The new character caused the screen to scroll up one row
                    if (cursorPos.column >= cursorLimits.column) { // The cursor was at the end of the row and should move to the beginning of the next row
                        Util::Graphic::Ansi::setPosition({0, cursorPos.row >= cursorLimits.row ? cursorPos.row : static_cast<uint16_t>(cursorPos.row + 1)});
                    } else { // The cursor just needs to advance one column, but we need to decrement the row, because the entire screen has been scrolled up
                        Util::Graphic::Ansi::setPosition({static_cast<uint16_t>(cursorPos.column + 1), static_cast<uint16_t>(cursorPos.row - 1)});
                    }
                } else { // The screen did not scroll
                    if (cursorPos.column < cursorLimits.column) { // The cursor just needs to advance one column
                        Util::Graphic::Ansi::setPosition({static_cast<uint16_t>(cursorPos.column + 1), cursorPos.row});
                    } else if (cursorPos.row < cursorLimits.row){ // The cursor was at the end of the line and should move to the beginning of the next row
                        Util::Graphic::Ansi::setPosition({0, static_cast<uint16_t>(cursorPos.row + 1)});
                    }
                }

                Util::Graphic::Ansi::enableRawMode();
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

    auto lfbSettingsString = Util::String();
    auto lfbFile = Util::Io::File("/device/lfb");
    if (lfbFile.exists()) {
        bool endOfFile;
        auto lfbInputStream = Util::Io::FileInputStream(lfbFile);
        lfbInputStream.readLine(endOfFile); // Skip address
        lfbSettingsString = lfbInputStream.readLine(endOfFile);
    }

    auto process = Util::Async::Process::execute(binaryFile, inputFile, outputFile, outputFile, command, arguments);
    if (!async) {
        process.join();

        if (lfbFile.exists()) {
            bool endOfFile;
            auto lfbInputStreamAfter = Util::Io::FileInputStream(lfbFile);
            lfbInputStreamAfter.readLine(endOfFile); // Skip address
            auto lfbSettingsStringAfter = lfbInputStreamAfter.readLine(endOfFile);
            if (lfbSettingsString != lfbSettingsStringAfter) {
                auto split1 = lfbSettingsString.split("x");
                auto split2 = split1[1].split("@");

                uint32_t resolutionX = Util::String::parseInt(split1[0]);
                uint32_t resolutionY = Util::String::parseInt(split2[0]);
                uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

                lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array({resolutionX, resolutionY, colorDepth}));
            }
        }

        Util::Graphic::Ansi::cleanupGraphicalApplication();
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

    Util::Io::File::controlFile(Util::Io::STANDARD_INPUT, Util::Graphic::Terminal::SET_ANSI_PARSING, {true});
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
    auto cursorPos = Util::Graphic::Ansi::getCursorPosition();
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto startPosition = getStartPosition();

    if (cursorPos.row <= startPosition.row && cursorPos.column > startPosition.column) {
        cursorPos.column -= 1;
    } else if (cursorPos.row > startPosition.row) {
        if (cursorPos.column == 0) {
            cursorPos.column = cursorLimits.column;
            cursorPos.row -= 1;
        } else {
            cursorPos.column -= 1;
        }
    }

    Util::Graphic::Ansi::setPosition(cursorPos);
}

void Shell::handleRightKey() {
    auto cursorPos = Util::Graphic::Ansi::getCursorPosition();
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto startPosition = getStartPosition();
    auto currentLineIndex = static_cast<uint32_t>((cursorPos.row - startPosition.row) * (cursorLimits.column + 1) + cursorPos.column - startPosition.column);

    if (currentLineIndex > currentLine.length() - 1) {
        return;
    }

    if (cursorPos.column < cursorLimits.column) {
        cursorPos.column += 1;
    } else {
        cursorPos.column = 0;
        cursorPos.row += 1;
    }

    Util::Graphic::Ansi::setPosition(cursorPos);
}

void Shell::handleBackspace() {
    if (currentLine.length() > 0) {
        handleLeftKey();

        auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
        auto cursorPos = Util::Graphic::Ansi::getCursorPosition();
        auto startPosition = getStartPosition();
        auto preCursor = currentLine.substring(0, (cursorPos.row - startPosition.row) * (cursorLimits.column + 1) + cursorPos.column - startPosition.column);
        auto afterCursor = currentLine.substring(((cursorPos.row - startPosition.row) * (cursorLimits.column + 1) + cursorPos.column - startPosition.column) + 1, currentLine.length());
        currentLine = preCursor + afterCursor;

        Util::System::out << afterCursor << ' ' << Util::Io::PrintStream::flush;
        Util::Graphic::Ansi::setPosition(cursorPos);
    }
}

void Shell::handleTab() {

}

uint32_t Shell::getScrolledLines() const {
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto currentScrolledRow = (startPosition.row + (currentLine.length() + startPosition.column) / (cursorLimits.column + 1));
    return currentScrolledRow > cursorLimits.row ? currentScrolledRow - cursorLimits.row : 0;
}

Util::Graphic::Ansi::CursorPosition Shell::getStartPosition() const {
    auto cursorPos = Util::Graphic::Ansi::getCursorPosition();
    auto scrolledLines = getScrolledLines();
    auto startPosition = Util::Graphic::Ansi::CursorPosition{Shell::startPosition.column, static_cast<uint16_t>(Shell::startPosition.row - scrolledLines)};
    if (startPosition.row > cursorPos.row) {
        startPosition.row = cursorPos.row;
    }

    return startPosition;
}
