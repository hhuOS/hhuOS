/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "TextDriver.h"

static uint32_t diff(uint32_t a, uint32_t b) {
    return a > b ? a - b : b - a;
}

uint16_t TextDriver::getColumnCount() {
    return columns;
}

uint16_t TextDriver::getRowCount() {
    return rows;
}

uint8_t TextDriver::getDepth() {
    return depth;
}

TextDriver::TextResolution TextDriver::findBestResolution(uint16_t columns, uint16_t rows, uint8_t depth) {
    Util::Array<TextResolution> resolutions = getTextResolutions();
    Util::ArrayList<TextResolution> candidates;

    uint32_t bestDiff = 0xffffffff;
    TextResolution bestRes{};

    // Find a resolution with the closest columns and rows to the desired columns and rows.
    for(const TextResolution &currentRes : resolutions) {
        uint32_t currentDiff = diff(columns, currentRes.columns) + diff(rows, currentRes.rows);

        if(currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestRes = currentRes;
        }
    }

    // Put all resolutions with the same columns and rows as bestRes in the candidates-list.
    for(const TextResolution &currentRes : resolutions) {
        if(currentRes.columns == bestRes.columns && currentRes.rows == bestRes.rows) {
            candidates.add(currentRes);
        }
    }

    // Find the resolution with the closest depth to the desired depth.
    bestDiff = 0xffffffff;
    for(const TextResolution &currentRes : candidates) {
        uint32_t currentDiff = diff(depth, currentRes.depth);

        if(currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestRes = currentRes;
        }
    }

    return bestRes;
}

void TextDriver::flush() {
    puts(StringBuffer::buffer, StringBuffer::pos);
    pos = 0;
}

bool TextDriver::init(uint16_t columns, uint16_t rows, uint8_t depth) {
    TextResolution res = findBestResolution(columns, rows, depth);

    bool ret = setResolution(res);

    if(ret) {
        this->columns = res.columns;
        this->rows = res.rows;
        this->depth = res.depth;

        clear();
    }

    return ret;
}