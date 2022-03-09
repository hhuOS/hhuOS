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

#ifndef HHUOS_SHELL_H
#define HHUOS_SHELL_H


#include "lib/util/async/Runnable.h"
#include "lib/util/stream/InputStream.h"
#include "lib/util/stream/OutputStream.h"

class Shell : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    Shell() = default;

    /**
     * Copy constructor.
     */
    Shell(const Shell &other) = delete;

    /**
     * Assignment operator.
     */
    Shell &operator=(const Shell &other) = delete;

    /**
     * Destructor.
     */
    ~Shell() override;

    void run() override;

private:

    void parseInput(const Util::Memory::String &input);

    Util::Memory::String checkPath(const Util::Memory::String &command);

    Util::Memory::String checkDirectory(const Util::Memory::String &command, const Util::File::File &directory);

    static void cd(const Util::Data::Array<Util::Memory::String> &arguments);

    static void executeBinary(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments, const Util::Memory::String &outputPath);

    Util::Stream::Reader *reader = nullptr;

    static const constexpr char *PATH = "/initrd/bin";
};


#endif