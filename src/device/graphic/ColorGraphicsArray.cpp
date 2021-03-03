#include <kernel/memory/MemLayout.h>
#include "ColorGraphicsArray.h"

namespace Device::Graphic {

ColorGraphicsArray::ColorGraphicsArray(uint16_t columns, uint16_t rows) : Terminal(columns, rows),
        cgaMemory(VIRT_CGA_START, columns * rows * BYTES_PER_CHARACTER), indexPort(INDEX_PORT_ADDRESS), dataPort(DATA_PORT_ADDRESS) {
    ColorGraphicsArray::clear();
}

void ColorGraphicsArray::putChar(char c) {
    uint16_t position = (currentRow * getColumns() + currentColumn) * BYTES_PER_CHARACTER;
    uint8_t colorAttribute = (bgColor.getRGB4() << 4) | fgColor.getRGB4();

    if (c == '\n') {
        currentRow++;
        currentColumn = 0;
    } else {
        cgaMemory.setByte(c, position);
        cgaMemory.setByte(colorAttribute, position + 1);
        currentColumn++;
    }

    if (currentColumn >= getColumns()) {
        currentRow++;
        currentColumn = 0;
    }

    if (currentRow >= getRows()) {
        scrollUp();
        currentColumn = 0;
        currentRow = getRows() - 1 ;
    }

    updateCursorPosition();
}

void ColorGraphicsArray::clear() {
    cgaMemory.setRange(0, getRows() * getColumns() * BYTES_PER_CHARACTER);

    currentRow = 0;
    currentColumn = 0;
    updateCursorPosition();
}

void ColorGraphicsArray::setPosition(uint16_t column, uint16_t row) {
    currentColumn = column;
    currentRow = row;
}

void ColorGraphicsArray::setForegroundColor(Util::Graphic::Color &color) {
    fgColor = color;
}

void ColorGraphicsArray::setBackgroundColor(Util::Graphic::Color &color) {
    bgColor = color;
}

void ColorGraphicsArray::updateCursorPosition() {
    uint16_t position = currentRow * getColumns() + currentColumn;

    // Set color attribute, so that the cursor will be visible
    uint8_t colorAttribute = (bgColor.getRGB4() << 4) | fgColor.getRGB4();
    cgaMemory.setByte(colorAttribute, position * BYTES_PER_CHARACTER + 1);

    auto low  = static_cast<uint8_t>(position & 0xff);
    auto high = static_cast<uint8_t>((position >> 8) & 0xff);

    // Write high byte
    indexPort.writeByte(CURSOR_HIGH_BYTE);
    dataPort.writeByte(high);

    // Write low byte
    indexPort.writeByte(CURSOR_LOW_BYTE);
    dataPort.writeByte(low);
}

void ColorGraphicsArray::scrollUp() {
    auto columns = getColumns();
    auto rows = getRows();

    // Move screen upwards by one row
    auto source = cgaMemory.add(columns * BYTES_PER_CHARACTER);
    cgaMemory.copyRange(source, columns * (rows - 1) * BYTES_PER_CHARACTER);

    // Clear last row
    auto clear = cgaMemory.add(columns * (rows - 1) * BYTES_PER_CHARACTER);
    clear.setRange(0, getColumns() * BYTES_PER_CHARACTER);
}

}