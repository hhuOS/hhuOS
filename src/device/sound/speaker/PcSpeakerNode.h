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

#ifndef HHUOS_PCSPEAKERNODE_H
#define HHUOS_PCSPEAKERNODE_H

#include <stdint.h>

#include "filesystem/memory/StringNode.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Device::Sound {

class PcSpeakerNode : public Filesystem::Memory::StringNode {

public:
    /**
     * Constructor.
     */
    explicit PcSpeakerNode(const Util::String &name);

    /**
     * Copy Constructor.
     */
    PcSpeakerNode(const PcSpeakerNode &other) = delete;

    /**
     * Assignment operator.
     */
    PcSpeakerNode &operator=(const PcSpeakerNode &other) = delete;

    /**
     * Destructor.
     */
    ~PcSpeakerNode() override = default;

    /**
     * Overriding function from StringNode.
     */
    Util::String getString() override;

    /**
     * Overriding function from MemoryNode.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    uint32_t currentFrequency = 0;
    Util::String buffer = "0\n";
};

}

#endif
