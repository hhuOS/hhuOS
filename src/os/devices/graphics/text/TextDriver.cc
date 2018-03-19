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
    Util::ArrayList<TextResolution>& resolutions = getTextResolutions();
    Util::ArrayList<TextResolution> candidates;

    uint32_t bestDiff = 0xffffffff;
    TextResolution bestRes{};

    // Find a resolution with the closest columns and rows to the desired columns and rows.
    for(TextResolution currentRes : resolutions) {
        uint32_t currentDiff = diff(columns, currentRes.columns) + diff(rows, currentRes.rows);

        if(currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestRes = currentRes;
        }
    }

    // Put all resolutions with the same columns and rows as bestRes in the candidates-list.
    for(TextResolution currentRes : resolutions) {
        if(currentRes.columns == bestRes.columns && currentRes.rows == bestRes.rows) {
            candidates.add(currentRes);
        }
    }

    // Find the resolution with the closest depth to the desired depth.
    bestDiff = 0xffffffff;
    for(TextResolution currentRes : candidates) {
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