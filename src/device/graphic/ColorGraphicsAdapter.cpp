#include "ColorGraphicsAdapter.h"

#include "lib/util/graphic/Color.h"
#include "lib/interface.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/Exception.h"
#include "device/system/Bios.h"
#include "kernel/process/Thread.h"

namespace Device::Graphic {

ColorGraphicsAdapter::ColorGraphicsAdapter(uint16_t columns, uint16_t rows) : Terminal(columns, rows), cgaMemory(mapBuffer(reinterpret_cast<void*>(CGA_START_ADDRESS), columns, rows)) {
    Terminal::clear();
    ColorGraphicsAdapter::setCursor(true);
}

ColorGraphicsAdapter::~ColorGraphicsAdapter() {
    delete reinterpret_cast<uint8_t*>(cgaMemory.get());
}

void ColorGraphicsAdapter::putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) {
    uint16_t position = (currentRow * getColumns() + currentColumn) * BYTES_PER_CHARACTER;
    uint8_t colorAttribute = (backgroundColor.getRGB4() << 4) | foregroundColor.getRGB4();

    cgaMemory.setByte(c, position);
    cgaMemory.setByte(colorAttribute, position + 1);
    currentColumn++;

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

void ColorGraphicsAdapter::clear(const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor, uint16_t startColumn, uint32_t startRow, uint16_t endColumn, uint16_t endRow) {
    uint8_t colorAttribute = (backgroundColor.getRGB4() << 4) | foregroundColor.getRGB4();
    uint16_t startPosition = (startRow * getColumns() + startColumn) * BYTES_PER_CHARACTER;
    uint16_t endPosition = (endRow * getColumns() + endColumn + 1) * BYTES_PER_CHARACTER;

    for (uint32_t i = startPosition; i < endPosition; i += 2) {
        cgaMemory.setByte(' ', i);
        cgaMemory.setByte(colorAttribute, i + 1);
    }
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
    clear(getForegroundColor(), getBackgroundColor(), 0, getRows() - 1, getColumns() - 1, getRows() - 1);
}

uint16_t ColorGraphicsAdapter::getCurrentColumn() const {
    return currentColumn;
}

uint16_t ColorGraphicsAdapter::getCurrentRow() const {
    return currentRow;
}

Util::Address<uint32_t> ColorGraphicsAdapter::getAddress() {
    return cgaMemory;
}

Util::Address<uint32_t> ColorGraphicsAdapter::mapBuffer(void *physicalAddress, uint16_t columns, uint16_t rows) {
    if (reinterpret_cast<uint32_t>(physicalAddress) % Util::PAGESIZE != 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "ColorGraphicsAdapter: Physical address is not page aligned!");
    }

    const auto size = columns * rows * BYTES_PER_CHARACTER;
    const auto pageCount = size % Util::PAGESIZE == 0 ? (size / Util::PAGESIZE) : (size / Util::PAGESIZE) + 1;
    void *virtualAddress = mapIO(physicalAddress, pageCount);

    return Util::Address<uint32_t>(virtualAddress);
}

ColorGraphicsAdapter::VideoCardType ColorGraphicsAdapter::getVideoCardType() {
    Kernel::Thread::Context biosContext{};
    biosContext.eax = BIOS_FUNCTION_CHECK_VIDEO_CARD;
    biosContext = Bios::interrupt(0x10, biosContext);

    return static_cast<VideoCardType>(biosContext.ebx);
}

void ColorGraphicsAdapter::setMode(ColorGraphicsAdapter::Mode mode) {
    Kernel::Thread::Context biosContext{};
    biosContext.eax = mode;
    Bios::interrupt(0x10, biosContext);
}

}