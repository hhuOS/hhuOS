#include "kernel/system/System.h"
#include "ColorGraphicsAdapter.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"

namespace Device::Graphic {

ColorGraphicsAdapter::ColorGraphicsAdapter(uint16_t columns, uint16_t rows) : Terminal(columns, rows),
        cgaMemory(Kernel::System::getService<Kernel::MemoryService>().mapIO(CGA_START_ADDRESS, static_cast<uint32_t>(columns * rows * 2))) {
    ColorGraphicsAdapter::clear(Util::Graphic::Colors::BLACK);
    ColorGraphicsAdapter::setCursor(true);
}

ColorGraphicsAdapter::~ColorGraphicsAdapter() {
    delete reinterpret_cast<uint8_t*>(cgaMemory.get());
}

void ColorGraphicsAdapter::putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) {
    uint16_t position = (currentRow * getColumns() + currentColumn) * BYTES_PER_CHARACTER;
    uint8_t colorAttribute = (backgroundColor.getRGB4() << 4) | foregroundColor.getRGB4();

    if (c == '\n') {
        cgaMemory.setByte(' ', position);
        cgaMemory.setByte(0, position + 1);
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
        currentRow = getRows() - 1;
    }

    updateCursorPosition();
}

void ColorGraphicsAdapter::clear(const Util::Graphic::Color &backgroundColor) {
    uint32_t size = getRows() * getColumns();
    for (uint32_t i = 0; i < size; i++) {
        cgaMemory.setShort(0x0700, i * 2);
    }

    currentRow = 0;
    currentColumn = 0;
    updateCursorPosition();
}

void ColorGraphicsAdapter::setPosition(uint16_t column, uint16_t row) {
    currentColumn = column;
    currentRow = row;

    while (currentRow >= getRows()) {
        scrollUp();
        currentRow--;
    }

    updateCursorPosition();
}

void ColorGraphicsAdapter::setCursor(bool enabled) {
    if (enabled) {
        indexPort.writeByte(CURSOR_START_INDEX);
        dataPort.writeByte(0x00);
        indexPort.writeByte(CURSOR_END_INDEX);
        dataPort.writeByte(0x1f);
    } else {
        indexPort.writeByte(CURSOR_START_INDEX);
        dataPort.writeByte(0x20);
    }
}

void ColorGraphicsAdapter::updateCursorPosition() {
    uint16_t position = currentRow * getColumns() + currentColumn;
    auto low  = static_cast<uint8_t>(position & 0xff);
    auto high = static_cast<uint8_t>((position >> 8) & 0xff);

    // Write high byte
    indexPort.writeByte(CURSOR_HIGH_BYTE);
    dataPort.writeByte(high);

    // Write low byte
    indexPort.writeByte(CURSOR_LOW_BYTE);
    dataPort.writeByte(low);
}

void ColorGraphicsAdapter::scrollUp() {
    auto columns = getColumns();
    auto rows = getRows();

    // Move screen upwards by one row
    auto source = cgaMemory.add(columns * BYTES_PER_CHARACTER);
    cgaMemory.copyRange(source, columns * (rows - 1) * BYTES_PER_CHARACTER);

    // Clear last row
    auto clear = cgaMemory.add(columns * (rows - 1) * BYTES_PER_CHARACTER);
    for (uint32_t i = 0; i < getColumns(); i++) {
        clear.setShort(0x0700, i * 2);
    }
}

uint16_t ColorGraphicsAdapter::getCurrentColumn() const {
    return currentColumn;
}

uint16_t ColorGraphicsAdapter::getCurrentRow() const {
    return currentRow;
}

Util::Memory::Address<uint32_t> ColorGraphicsAdapter::getAddress() {
    return cgaMemory;
}

}