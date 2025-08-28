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

#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/System.h"
#include "lib/util/async/Process.h"
#include "lib/util/graphic/Terminal.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/collection/ArrayList.h"
#include "CommandLine.h"

CommandLine::CommandLine(const Util::String &path) : startDirectory(path) {}

void CommandLine::run() {
    if (!Util::Io::File::changeDirectory(startDirectory)) {
        Util::System::error << "Unable to start shell in '" << startDirectory << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
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

void CommandLine::runCommand(Util::String command) {
	if (!Util::Io::File::changeDirectory(startDirectory)) {
        Util::System::error << "Unable to start shell in '" << startDirectory << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }
	
	currentLine = command;
	parseInput();
}


void CommandLine::beginCommandLine() {
    currentLine = "";
    auto currentDirectory = Util::Io::File::getCurrentWorkingDirectory();

    buildAutoCompletionLists();

    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN << "["
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_WHITE << (currentDirectory.getCanonicalPath().isEmpty() ? "/" : currentDirectory.getName())
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN << "]> "
                      << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Io::PrintStream::flush;

    startPosition = Util::Graphic::Ansi::getCursorPosition();
}

void CommandLine::readLine() {
    Util::Graphic::Ansi::enableRawMode();
    int16_t input = Util::Graphic::Ansi::readChar();

    while (isRunning) {
        switch (input) {
            case Util::Graphic::Ansi::UP:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleUpKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::DOWN:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleDownKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::RIGHT:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleRightKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::LEFT:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleLeftKey();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::POS1:
                Util::Graphic::Ansi::enableCanonicalMode();
                handlePos1();
                Util::Graphic::Ansi::enableRawMode();
                break;
            case Util::Graphic::Ansi::END:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleEnd();
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
                Util::System::out << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;

                currentLine = currentLine.strip();
                return;
            }
            case 0x7f:
                Util::Graphic::Ansi::enableCanonicalMode();
                handleDel();
                Util::Graphic::Ansi::enableRawMode();
            case 0x00 ... 0x06:
            case 0x0b ... 0x1f:
                break;
            default:
                Util::Graphic::Ansi::enableCanonicalMode();
                auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
                auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
                auto scrolledLines = getScrolledLines(); // Amount of lines the screen has been scrolled to display the current line
                auto startPosition = getStartPosition();
                auto preCursor = currentLine.substring(0, (cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column);
                auto afterCursor = currentLine.substring((cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column, currentLine.length());
                currentLine = preCursor + static_cast<char>(input) + afterCursor;

                Util::System::out << static_cast<char>(input) << afterCursor << Util::Io::PrintStream::flush;

                if (getScrolledLines() > scrolledLines) { // The new character caused the screen to scroll up one row
                    if (cursorPosition.column >= cursorLimits.column) { // The cursor was at the end of the row and should move to the beginning of the next row
                        Util::Graphic::Ansi::setPosition({0, cursorPosition.row >= cursorLimits.row ? cursorPosition.row : static_cast<uint16_t>(cursorPosition.row + 1)});
                    } else { // The cursor just needs to advance one column, but we need to decrement the row, because the entire screen has been scrolled up
                        Util::Graphic::Ansi::setPosition({static_cast<uint16_t>(cursorPosition.column + 1), static_cast<uint16_t>(cursorPosition.row - 1)});
                    }
                } else { // The screen did not scroll
                    if (cursorPosition.column < cursorLimits.column) { // The cursor just needs to advance one column
                        Util::Graphic::Ansi::setPosition({static_cast<uint16_t>(cursorPosition.column + 1), cursorPosition.row});
                    } else if (cursorPosition.row < cursorLimits.row){ // The cursor was at the end of the line and should move to the beginning of the next row
                        Util::Graphic::Ansi::setPosition({0, static_cast<uint16_t>(cursorPosition.row + 1)});
                    }
                }

                Util::Graphic::Ansi::enableRawMode();
        }

        if (input != '\t') {
            autoCompletionSearchString = "";
            autoCompletionIndex = 0;
        }

        input = Util::Graphic::Ansi::readChar();
    }

    Util::Graphic::Ansi::enableCanonicalMode();
}

void CommandLine::parseInput() {
    const auto async = currentLine.endsWith("&");
    const auto pipeSplit = currentLine.substring(0, async ? currentLine.length() - 1 : currentLine.length()).split(">");

    if (pipeSplit.length() == 0) {
        return;
    }

    const auto command = pipeSplit[0].substring(0, currentLine.indexOf(" "));
    const auto rest = pipeSplit[0].substring(currentLine.indexOf(" "), currentLine.length());

    bool valid;
    auto arguments = parseArguments(rest.strip(), valid);
    const auto targetFile = pipeSplit.length() == 1 ? "/device/terminal" : pipeSplit[1].split(" ")[0];

    if (!valid) {
        Util::System::out << "Invalid argument string!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    } else if (command == "cd") {
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

Util::Array<Util::String> CommandLine::parseArguments(const Util::String &argumentString, bool &valid) {
    auto argumentList = Util::ArrayList<Util::String>();
    auto currentArgument = Util::String();
    bool inString = false;
    for (uint32_t i = 0; i < argumentString.length(); i++) {
        auto currentCharacter = argumentString[i];
        if (currentCharacter == '"') {
            if (currentArgument.length() > 0) {
                argumentList.add(currentArgument);
                currentArgument = "";
            }

            inString = !inString;
        } else if (currentCharacter == ' ' && !inString) {
            if (currentArgument.length() > 0) {
                argumentList.add(currentArgument);
                currentArgument = "";
            }
        } else {
            currentArgument += currentCharacter;
        }
    }

    if (!currentArgument.isEmpty()) {
        argumentList.add(currentArgument);
    }

    valid = !inString;
    return argumentList.toArray();
}

Util::String CommandLine::checkPath(const Util::String &command) const {
    for (const auto &path : Util::String(PATH).split(":")) {
        auto file = Util::Io::File(path);
        auto binaryPath = checkDirectory(command, file);
        if (!binaryPath.isEmpty()) {
            return binaryPath;
        }
    }

    return "";
}

Util::String CommandLine::checkDirectory(const Util::String &command, Util::Io::File &directory) const {
    if (!directory.exists() || !directory.isDirectory()) {
        return "";
    }

    for (auto &file : directory.getChildren()) {
        if (file.isFile() && file.getName() == command) {
            return file.getCanonicalPath();
        }

        if (file.isDirectory()) {
            checkDirectory(command, file);
        }
    }

    return "";
}

void CommandLine::cd(const Util::Array<Util::String> &arguments) {
    if (arguments.length() == 0) {
        return;
    }

    const auto &path = arguments[0];
    auto file = Util::Io::File(path);

    if (!file.exists()) {
        Util::System::out << "cd: '" << path << "' not found!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }

    if (file.isFile()) {
        Util::System::out << "cd: '" << path << "' is not a directory!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }

    auto result = Util::Io::File::changeDirectory(path);
    if (!result) {
        Util::System::out << "cd: Failed to change directory!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }
}

void CommandLine::executeBinary(const Util::String &path, const Util::String &command, const Util::Array<Util::String> &arguments, const Util::String &outputPath, bool async) {
    auto binaryFile = Util::Io::File(path);
    auto inputFile = Util::Io::File("/device/terminal");
    auto outputFile = Util::Io::File(outputPath);

    if (!binaryFile.exists()) {
        Util::System::out << "'" << path << "' not found!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }

    if (binaryFile.isDirectory()) {
        Util::System::out << "'" << path << "' is a directory!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }

    if (!outputFile.exists() && !outputFile.create(Util::Io::File::REGULAR)) {
        Util::System::out << "Failed to execute file '" << path << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return;
    }

    auto lfbSettingsString = Util::String();
    auto lfbFile = Util::Io::File("/device/lfb");
    if (lfbFile.exists()) {
        auto lfbInputStream = Util::Io::FileInputStream(lfbFile);
        lfbInputStream.readLine(); // Skip address
        lfbSettingsString = lfbInputStream.readLine();
    }

    auto process = Util::Async::Process::execute(binaryFile, inputFile, outputFile, outputFile, command, arguments);
    if (!async) {
        process.join();

        if (lfbFile.exists()) {
            auto lfbInputStreamAfter = Util::Io::FileInputStream(lfbFile);
            lfbInputStreamAfter.readLine(); // Skip address
            auto lfbSettingsStringAfter = lfbInputStreamAfter.readLine();
            if (lfbSettingsString != lfbSettingsStringAfter) {
                auto split1 = lfbSettingsString.split("x");
                auto split2 = split1[1].split("@");

                auto resolutionX = Util::String::parseNumber<uint16_t>(split1[0]);
                auto resolutionY = Util::String::parseNumber<uint16_t>(split2[0]);
                uint8_t colorDepth = split1.length() > 1 ? Util::String::parseNumber<uint8_t>(split2[1]) : 32;

                lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
            }
        }

        Util::Graphic::Ansi::cleanupGraphicalApplication();
    }
}

void CommandLine::handleUpKey() {
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

void CommandLine::handleDownKey() {
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

void CommandLine::handleLeftKey() {
    auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto startPosition = getStartPosition();

    if (cursorPosition.row <= startPosition.row && cursorPosition.column > startPosition.column) {
        cursorPosition.column -= 1;
    } else if (cursorPosition.row > startPosition.row) {
        if (cursorPosition.column == 0) {
            cursorPosition.column = cursorLimits.column;
            cursorPosition.row -= 1;
        } else {
            cursorPosition.column -= 1;
        }
    }

    Util::Graphic::Ansi::setPosition(cursorPosition);
}

void CommandLine::handleRightKey() {
    auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto startPosition = getStartPosition();
    auto currentLineIndex = static_cast<uint32_t>((cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column);

    if (currentLineIndex > currentLine.length() - 1) {
        return;
    }

    if (cursorPosition.column < cursorLimits.column) {
        cursorPosition.column += 1;
    } else {
        cursorPosition.column = 0;
        cursorPosition.row += 1;
    }

    Util::Graphic::Ansi::setPosition(cursorPosition);
}

void CommandLine::handleBackspace() {
    auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
    auto startPosition = getStartPosition();

    if (currentLine.length() == 0 || cursorPosition == startPosition) {
        return;
    }

    handleLeftKey();
    cursorPosition = Util::Graphic::Ansi::getCursorPosition();

    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto preCursor = currentLine.substring(0, (cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column);
    auto afterCursor = currentLine.substring(((cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column) + 1, currentLine.length());

    currentLine = preCursor + afterCursor;

    Util::System::out << afterCursor << ' ' << Util::Io::PrintStream::flush;
    Util::Graphic::Ansi::setPosition(cursorPosition);
}

void CommandLine::handleDel() {
    auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();

    auto preCursor = currentLine.substring(0, (cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column);
    auto afterCursor = currentLine.substring(((cursorPosition.row - startPosition.row) * (cursorLimits.column + 1) + cursorPosition.column - startPosition.column) + 1, currentLine.length());

    currentLine = preCursor + afterCursor;

    Util::System::out << afterCursor << ' ' << Util::Io::PrintStream::flush;
    Util::Graphic::Ansi::setPosition(cursorPosition);
}

void CommandLine::handleTab() {
    if (currentLine.isEmpty() || currentLine.contains(' ')) {
        return;
    }

    if (autoCompletionSearchString.isEmpty()) {
        autoCompletionSearchString = currentLine;
    }

    auto autoCompletionSuggestions = Util::ArrayList<Util::String>();
    for (const auto &fileName : autoCompletionPathSuggestions) {
        autoCompletionSuggestions.add(fileName);
    }
    for (const auto &fileName : autoCompletionCurrentWorkingDirectorySuggestions) {
        autoCompletionSuggestions.add(fileName);
    }

    for (uint32_t i = 0; i < autoCompletionSuggestions.size(); i++) {
        auto index = (autoCompletionIndex + i) % autoCompletionSuggestions.size();
        const auto &suggestion = autoCompletionSuggestions.get(index);

        if (suggestion.beginsWith(autoCompletionSearchString)) {
            autoCompletionIndex = index + 1 % autoCompletionSuggestions.size();
            currentLine = suggestion;

            auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
            Util::Graphic::Ansi::setPosition(startPosition);

            for (auto row = startPosition.row; row <= cursorPosition.row; row++) {
                if (row == startPosition.row) {
                    Util::Graphic::Ansi::clearLineFromCursor();
                } else {
                    if (row == cursorPosition.row) {
                        Util::Graphic::Ansi::clearLineToCursor();
                    } else {
                        Util::Graphic::Ansi::clearLine();
                    }

                    Util::Graphic::Ansi::setPosition(Util::Graphic::Ansi::CursorPosition{cursorPosition.column, static_cast<uint16_t>(row + 1)});
                }
            }

            Util::Graphic::Ansi::setPosition(startPosition);
            Util::System::out << suggestion << Util::Io::PrintStream::flush;

            break;
        }
    }
}

void CommandLine::handlePos1() {
    Util::Graphic::Ansi::setPosition(getStartPosition());
}

void CommandLine::handleEnd() {
    Util::Graphic::Ansi::setPosition(getEndPosition());
}

uint32_t CommandLine::getScrolledLines() const {
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();
    auto currentScrolledRow = (startPosition.row + (currentLine.length() + startPosition.column) / (cursorLimits.column + 1));
    return currentScrolledRow > cursorLimits.row ? currentScrolledRow - cursorLimits.row : 0;
}

Util::Graphic::Ansi::CursorPosition CommandLine::getStartPosition() const {
    auto cursorPosition = Util::Graphic::Ansi::getCursorPosition();
    auto scrolledLines = getScrolledLines();
    auto startPosition = Util::Graphic::Ansi::CursorPosition{CommandLine::startPosition.column, static_cast<uint16_t>(CommandLine::startPosition.row - scrolledLines)};
    if (startPosition.row > cursorPosition.row) {
        startPosition.row = cursorPosition.row;
    }

    return startPosition;
}

Util::Graphic::Ansi::CursorPosition CommandLine::getEndPosition() const {
    auto position = getStartPosition();
    auto cursorLimits = Util::Graphic::Ansi::getCursorLimits();

    position.column += currentLine.length();
    while (position.column > cursorLimits.column) {
        position.column -= (cursorLimits.column + 1);
        position.row++;
    }

    return position;
}

void CommandLine::buildAutoCompletionLists() {
    autoCompletionPathSuggestions.clear();
    autoCompletionCurrentWorkingDirectorySuggestions.clear();

    for (const auto &path : Util::String(PATH).split(":")) {
        auto directory = Util::Io::File(path);
        if (directory.exists() && directory.isDirectory()) {
            for (auto &file : directory.getChildren()) {
                if (file.isFile()) {
                    autoCompletionPathSuggestions.add(file.getName());
                }
            }
        }
    }

    auto workingDirectory = Util::Io::File::getCurrentWorkingDirectory();
    for (auto &file : workingDirectory.getChildren()) {
        if (file.isFile()) {
            autoCompletionCurrentWorkingDirectorySuggestions.add(file.getName());
        }
    }
}
