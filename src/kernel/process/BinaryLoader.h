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

#ifndef HHUOS_BINARYLOADER_H
#define HHUOS_BINARYLOADER_H

#include "lib/util/async/Runnable.h"
#include "lib/util/memory/String.h"
#include "lib/util/data/Array.h"

namespace Kernel {

class BinaryLoader : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    explicit BinaryLoader(const Util::Memory::String &path, const Util::Memory::String &command, const Util::Data::Array<Util::Memory::String> &arguments);

    /**
     * Copy Constructor.
     */
    BinaryLoader(const BinaryLoader &other) = delete;

    /**
     * Assignment operator.
     */
    BinaryLoader &operator=(const BinaryLoader &other) = delete;

    /**
     * Destructor.
     */
    ~BinaryLoader() override = default;

    void run() override;

private:

    const Util::Memory::String path;
    const Util::Memory::String command;
    const Util::Data::Array<Util::Memory::String> arguments;
};

}

#endif