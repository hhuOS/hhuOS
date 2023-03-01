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

#ifndef HHUOS_SHELL_H
#define HHUOS_SHELL_H


#include <cstdint>

#include "lib/util/async/Runnable.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Io {
class File;
}  // namespace File
}  // namespace Util

class Shell : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    explicit Shell(const Util::String &path = "/");

    /**
     * Copy Constructor.
     */
    Shell(const Shell &other) = delete;

    /**
     * Assignment operator.
     */
    Shell &operator=(const Shell &other) = delete;

    /**
     * Destructor.
     */
    ~Shell() override = default;

    void run() override;

private:

    void beginCommandLine();

    void readLine();

    void handleUpKey();

    void handleDownKey();

    void handleLeftKey();

    void handleRightKey();

    void handleBackspace();

    void handleTab();

    void parseInput();

    [[nodiscard]] Util::String checkPath(const Util::String &command) const;

    Util::String checkDirectory(const Util::String &command, Util::Io::File &directory) const;

    static void cd(const Util::Array<Util::String> &arguments);

    static void executeBinary(const Util::String &path, const Util::String &command, const Util::Array<Util::String> &arguments, const Util::String &outputPath, bool async);

    bool isRunning = true;
    Util::String startDirectory;
    Util::String currentLine;
    Util::String historyCurrentLine;
    Util::Graphic::Ansi::CursorPosition startPosition{};

    Util::ArrayList<Util::String> history;
    uint32_t historyIndex = 0;

    static const constexpr char *PATH = "/initrd/bin:/bin";
};


#endif