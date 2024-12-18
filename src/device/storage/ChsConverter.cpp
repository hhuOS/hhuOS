/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "ChsConverter.h"

ChsConverter::ChsConverter(uint8_t cylinders, uint8_t heads, uint8_t sectorsPerCylinder) : cylinders(cylinders), heads(heads), sectorsPerTrack(sectorsPerCylinder) {}

ChsConverter::CylinderHeadSector ChsConverter::lbaToChs(uint32_t lbaSector) const {
    return {
            static_cast<uint16_t>(lbaSector / (heads * sectorsPerTrack)),
            static_cast<uint8_t>((lbaSector % (heads * sectorsPerTrack)) / sectorsPerTrack),
            static_cast<uint8_t>((lbaSector % sectorsPerTrack) + 1)
    };
}
