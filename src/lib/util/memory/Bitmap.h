/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_BITMAP_H
#define HHUOS_BITMAP_H

#include <cstdint>

namespace Util::Memory {

class Bitmap {

protected:

    uint32_t *bitmap = nullptr;

    uint32_t arraySize = 0;

    uint32_t blocks = 0;

public:

    Bitmap() = default;

    explicit Bitmap(uint32_t blockCount);

    Bitmap(uint32_t blockCount, uint32_t reservedBlocksAtBeginning);

    Bitmap(const Bitmap &copy) = delete;

    Bitmap &operator=(const Bitmap &other) = delete;

    virtual ~Bitmap();

    [[nodiscard]] uint32_t getSize() const;

    virtual void set(uint32_t block);

    virtual void unset(uint32_t block);

    virtual bool check(uint32_t block, bool set);

    virtual void setRange(uint32_t startBlock, uint32_t length);

    virtual void unsetRange(uint32_t startBlock, uint32_t length);

    virtual bool checkRange(uint32_t startBlock, uint32_t length, bool set);

    virtual uint32_t findAndSet(uint32_t blockCount);

private:

    uint32_t findNext(uint32_t startBlock, bool set);
};

}

#endif
