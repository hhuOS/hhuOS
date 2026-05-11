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

#ifndef HHUOS_APPLICATION_PEANUT_GB_H
#define HHUOS_APPLICATION_PEANUT_GB_H

#include <stdint.h>

struct gb_s;

/// Get the color palette for the game currently played by the `gb_s` struct.
/// If no predefined color palette exists for the given game, the default palette is returned.
/// The palette consists of twelve 32-bit values:
///     - Four values for OBJ0 (Object 0)
///     - Four values for OBJ1 (Object 1)
///     - Four values for BG (Background)
/// Although each value is 32 bit wide, the actual width is determined by the `colorDepth` parameter.
/// For example, if `colorDepth` is 16, the resulting palette still consists of 32-bit values, but only
/// the least significant 16 bits of the values are valid.
/// In Peanut-GB, each pixel is given as an 8-bit value with bits 4-5 determining the object type (OBJ0, OBJ1, BG)
/// and bits 1-2 determining the pixel color (i.e. index into the object color palette).
/// A simple 4-bit index into the twelve-color palette can be calculated with the following formula:
/// ```c++
/// (pixel >> 2) | (pixel & 0x03)
/// ```
/// The result can be used to extract a 32-bit color value from a palette.
uint32_t* getPalette(gb_s *gb, uint8_t colorDepth);

/// Get a color palette for a specific game.
/// The ROM title and color hash can be extracted using the `gb_get_rom_name()` and `gb_color_hash()` from Peanut-GB.
/// A shortcut for this is provided by the function `getPalette()`.
/// If no predefined color palette exists for the given game, the default palette is returned.
uint32_t* getPaletteForRom(const char* romTitle, uint8_t colorHash, uint8_t colorDepth);

/// Get one of the predefined color palettes that users could choose via button combinations on real Game Boy Colors.
/// The following indices are valid:
///     0 -> Black and White
///     1 -> Shades of Green
///     2 -> CGB Palette when pressing Up
///     3 -> CGB Palette when pressing A + Up
///     4 -> CGB Palette when pressing B + Up
///     5 -> CGB Palette when pressing Left
///     6 -> CGB Palette when pressing A + Left
///     7 -> CGB Palette when pressing B + Left
///     8 -> CGB Palette when pressing Down
///     9 -> CGB Palette when pressing A + Down
///     10 -> CGB Palette when pressing B + Down
///     11 -> CGB Palette when pressing Right
///     12 -> CGB Palette when pressing A + Right
///     13 -> CGB Palette when pressing B + Right
uint32_t* getManualPalette(uint8_t manualIndex, uint8_t colorDepth);

#endif