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

#ifndef HHUOS_COMMANDLINE_H
#define HHUOS_COMMANDLINE_H


#include <stdint.h>

#include "lib/util/async/Runnable.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Io {
class File;
}  // namespace File
}  // namespace Util

class CommandLine : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    explicit CommandLine(const Util::String &path = "/");

    /**
     * Copy Constructor.
     */
    CommandLine(const CommandLine &other) = delete;

    /**
     * Assignment operator.
     */
    CommandLine &operator=(const CommandLine &other) = delete;

    /**
     * Destructor.
     */
    ~CommandLine() override = default;

    void run() override;
	
	void runCommand(Util::String command);

private:

    void beginCommandLine();

    void readLine();

    void handleUpKey();

    void handleDownKey();

    void handleLeftKey();

    void handleRightKey();

    void handleBackspace();

    void handleDel();

    void handleTab();

    void handlePos1();

    void handleEnd();

    void parseInput();

    Util::Array<Util::String> parseArguments(const Util::String &argumentString, bool &valid);

    [[nodiscard]] uint32_t getScrolledLines() const;

    [[nodiscard]] Util::Graphic::Ansi::CursorPosition getStartPosition() const;

    [[nodiscard]] Util::Graphic::Ansi::CursorPosition getEndPosition() const;

    [[nodiscard]] Util::String checkPath(const Util::String &command) const;

    Util::String checkDirectory(const Util::String &command, Util::Io::File &directory) const;

    void buildAutoCompletionLists();

    static void cd(const Util::Array<Util::String> &arguments);

    static void executeBinary(const Util::String &path, const Util::String &command, const Util::Array<Util::String> &arguments, const Util::String &outputPath, bool async);

    bool isRunning = true;
    Util::String startDirectory;
    Util::String currentLine;
    Util::String historyCurrentLine;
    Util::Graphic::Ansi::CursorPosition startPosition{};

    Util::ArrayList<Util::String> history;
    uint32_t historyIndex = 0;

    Util::String autoCompletionSearchString = "";
    Util::ArrayList<Util::String> autoCompletionPathSuggestions;
    Util::ArrayList<Util::String> autoCompletionCurrentWorkingDirectorySuggestions;
    uint32_t autoCompletionIndex = 0;

    static const constexpr char *PATH = "/bin";
};


#endif