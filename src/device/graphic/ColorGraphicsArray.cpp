#include <kernel/memory/MemLayout.h>
#include "ColorGraphicsArray.h"

namespace Device::Graphic {

const Util::Memory::Address<uint32_t> ColorGraphicsArray::CGA_MEMORY(VIRT_CGA_START);

ColorGraphicsArray::ColorGraphicsArray(uint16_t columns, uint16_t rows) : Terminal(columns, rows), indexPort(INDEX_PORT_ADDRESS), dataPort(DATA_PORT_ADDRESS) {
    ColorGraphicsArray::clear();
}

void ColorGraphicsArray::putChar(char c) {
    uint16_t position = (currentRow * getColumns() + currentColumn) * BYTES_PER_CHARACTER;
    uint8_t colorAttribute = (bgColor.getRGB4() << 4) | fgColor.getRGB4();

    if (c == '\n') {
        currentRow++;
        currentColumn = 0;
    } else {
        CGA_MEMORY.setByte(position, c);
        CGA_MEMORY.setByte(position + 1, colorAttribute);
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
    CGA_MEMORY.setRange(0, getRows() * getColumns() * BYTES_PER_CHARACTER);

    currentRow = 0;
    currentColumn = 0;
    // updateCursorPosition();
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
    CGA_MEMORY.setByte(position * BYTES_PER_CHARACTER + 1, colorAttribute);

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
    auto source = Util::Memory::Address<uint32_t>(CGA_MEMORY.get() + columns * BYTES_PER_CHARACTER);
    CGA_MEMORY.copyRange(source, columns * (rows - 1) * BYTES_PER_CHARACTER);

    // Clear last row
    auto clear = Util::Memory::Address<uint32_t>(CGA_MEMORY.get() + columns * (rows - 1) * BYTES_PER_CHARACTER);
    clear.setRange(0, getColumns() * BYTES_PER_CHARACTER);
}

}