/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_SMBIOSDRIVER_H
#define HHUOS_SMBIOSDRIVER_H

#include "filesystem/memory/MemoryDriver.h"
#include "lib/util/io/file/File.h"

namespace Filesystem::SmBios {

class SmBiosDriver : public Memory::MemoryDriver {

public:
    /**
     * Default Constructor.
     */
    SmBiosDriver();

    /**
     * Copy Constructor.
     */
    SmBiosDriver(const SmBiosDriver &other) = delete;

    /**
     * Assignment operator.
     */
    SmBiosDriver& operator=(const SmBiosDriver &other) = delete;

    /**
     * Destructor.
     */
    ~SmBiosDriver() override = default;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool createNode(const Util::String &path, Util::Io::File::Type type) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool deleteNode(const Util::String &path) override;
};

}

#endif
