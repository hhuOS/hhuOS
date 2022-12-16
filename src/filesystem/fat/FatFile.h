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

#ifndef HHUOS_FATFILE_H
#define HHUOS_FATFILE_H

#include <cstdint>

#include "FatNode.h"
#include "filesystem/fat/ff/source/ff.h"
#include "lib/util/data/Array.h"
#include "lib/util/file/Type.h"

namespace Util {
namespace Memory {
class String;
}  // namespace Memory
}  // namespace Util

namespace Filesystem::Fat {

class FatFile : public FatNode {

public:
    /**
     * Constructor.
     */
    FatFile(FIL *file, FILINFO *info);

    /**
     * Copy Constructor.
     */
    FatFile(const FatFile &other) = delete;

    /**
     * Assignment operator.
     */
    FatFile &operator=(const FatFile &other) = delete;

    /**
     * Destructor.
     */
    ~FatFile() override;

    /**
     * Overriding function from Node.
     */
    Util::File::Type getFileType() override;

    /**
     * Overriding function from Node.
     */
    Util::Data::Array <Util::Memory::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    FIL *file;
};

}

#endif
