#include <util/memory/Address.h>
#include "LinearFrameBufferTerminal.h"

namespace Util::Graphic {

LinearFrameBufferTerminal::LinearFrameBufferTerminal(LinearFrameBuffer &lfb, Font &font, char cursor) :
        Terminal(lfb.getResolutionX() / font.getCharWidth(), lfb.getResolutionY() / font.getCharHeight()),
        cursor(cursor), lfb(lfb), scroller(lfb), pixelDrawer(lfb), stringDrawer(pixelDrawer), font(font) {
    LinearFrameBufferTerminal::clear();
    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), cursor, fgColor, bgColor);
}

void LinearFrameBufferTerminal::putChar(char c) {
    if (c == '\n') {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), ' ', fgColor, bgColor);
        currentRow++;
        currentColumn = 0;
    } else {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, fgColor, bgColor);
        currentColumn++;
    }

    if (currentColumn >= getColumns()) {
        currentRow++;
        currentColumn = 0;
    }

    if (currentRow >= getRows()) {
        scroller.scrollUp(font.getCharHeight());
        currentColumn = 0;
        currentRow = getRows() - 1 ;
    }

    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), cursor, fgColor, bgColor);
}

void LinearFrameBufferTerminal::clear() {
    lfb.clear();

    currentRow = 0;
    currentColumn = 0;
}

void LinearFrameBufferTerminal::setPosition(uint16_t column, uint16_t row) {
    currentColumn = column;
    currentRow = row;
}

void LinearFrameBufferTerminal::setForegroundColor(Color &color) {
    fgColor = color;
}

void LinearFrameBufferTerminal::setBackgroundColor(Color &color) {
    bgColor = color;
}

LinearFrameBuffer &LinearFrameBufferTerminal::getLinearFrameBuffer() const {
    return lfb;
}

}