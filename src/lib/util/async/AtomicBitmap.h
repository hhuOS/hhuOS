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

#ifndef HHUOS_ATOMICBITMAP_H
#define HHUOS_ATOMICBITMAP_H

#include <cstdint>

namespace Util::Async {

class AtomicBitmap {

public:

    AtomicBitmap() = default;

    explicit AtomicBitmap(uint32_t blockCount);

    AtomicBitmap(const AtomicBitmap &copy) = delete;

    AtomicBitmap &operator=(const AtomicBitmap &other) = delete;

    ~AtomicBitmap() = default;

    [[nodiscard]] uint32_t getSize() const;

    void set(uint32_t block);

    void unset(uint32_t block);

    bool check(uint32_t block, bool set);

    uint32_t findAndSet();

    uint32_t findAndUnset();

    static const constexpr uint32_t INVALID_INDEX = 0xffffffff;

private:

    uint32_t *bitmap = nullptr;
    uint32_t arraySize = 0;
    uint32_t blocks = 0;

};

}

#endif