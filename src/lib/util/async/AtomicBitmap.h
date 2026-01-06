/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_ASYNC_ATOMICBITMAP_H
#define HHUOS_LIB_UTIL_ASYNC_ATOMICBITMAP_H

#include <stdint.h>
#include <stddef.h>

namespace Util {
namespace Async {

/// A bitmap that can be used to manage a set of blocks (e.g. page frames).
/// It uses atomic operations to ensure thread-safety.
///
/// ## Example
/// ```c++
/// auto bitmap = Util::Async::AtomicBitmap(8); // bitmap = 00000000
/// bitmap.set(0); // Set first bit to 1 (bitmap = 10000000)
/// const auto set1 = bitmap.findAndSet(); // (bitmap = 11000000, set1 = 1)
/// bitmap.unset(0); // Unset first bit again (bitmap = 01000000)
/// const auto set2 = bitmap.findAndSet(); // (bitmap = 11000000, set2 = 0)
/// bitmap.unset(0); // Unset first bit again (bitmap = 01000000)
/// const auto unset1 = bitmap.findAndUnset(); // (bitmap = 00000000, unset1 = 0)
/// ```
class AtomicBitmap {

public:
    /// Create a new bitmap with the given number of blocks (bits).
    /// The bitmap is allocated on the heap and deleted in the destructor.
    explicit AtomicBitmap(size_t blockCount);

    /// Bitmaps should not be copied, since copies would share the same memory.
    AtomicBitmap(const AtomicBitmap &copy) = delete;

    /// Bitmaps should not be copied, since copies would share the same memory.
    AtomicBitmap& operator=(const AtomicBitmap &other) = delete;

    /// Destructor that frees the allocated memory.
    ~AtomicBitmap();

    /// Get the number of blocks (bits) in the bitmap.
    size_t getSize() const {
        return blocks;
    }

    /// Set the bit at the given index to 1.
    /// If the index is out of bounds, a panic is fired.
    void set(size_t block) const;

    /// Set the bit at the given index to 0.
    /// If the index is out of bounds, a panic is fired.
    void unset(size_t block) const;

    /// Check if the bit at the given index is set/unset.
    /// The boolean value `set` indicates whether to check for set or unset.
    /// If the index is out of bounds, a panic is fired.
    bool check(size_t block) const;

    /// Find the first unset bit and set it to 1.
    /// Return the index of the bit that was set or INVALID_INDEX if no unset bit was found.
    size_t findAndSet() const;

    /// Find the first set bit and set it to 0.
    /// Return the index of the bit that was unset or INVALID_INDEX if no set bit was found.
    size_t findAndUnset() const;

    /// Indicates that no suitable bit was found.
    static constexpr size_t INVALID_INDEX = SIZE_MAX;

private:

    size_t *bitmap = nullptr;
    size_t arraySize = 0;
    size_t blocks = 0;

    static constexpr size_t SIZE_BITS = sizeof(size_t) * 8;
};

}
}

#endif