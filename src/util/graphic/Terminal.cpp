#include <util/memory/Address.h>
#include "Terminal.h"

namespace Util::Graphic {

Terminal::Terminal(LinearFrameBuffer &lfb, Font &font, char cursor) : columns(lfb.getResolutionX() / font.getCharWidth()), rows(lfb.getResolutionY() / font.getCharHeight()),
        cursor(cursor), scroller(lfb), pixelDrawer(lfb), stringDrawer(pixelDrawer), font(font) {
    lfb.clear();
    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), cursor, fgColor, bgColor);
}

void Terminal::putChar(char c) {
    if (c == '\n') {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), ' ', fgColor, bgColor);
        currentRow++;
        currentColumn = 0;
    } else {
        stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), c, fgColor, bgColor);
        currentColumn++;
    }

    if (currentColumn >= columns) {
        currentRow++;
        currentColumn = 0;
    }

    if (currentRow >= rows) {
        scroller.scrollUp(font.getCharHeight());
        currentColumn = 0;
        currentRow = rows - 1 ;
    }

    stringDrawer.drawChar(font, currentColumn * font.getCharWidth(), currentRow * font.getCharHeight(), cursor, fgColor, bgColor);
}

void Terminal::putString(const char *string) {
    for (uint32_t i = 0; string[i] != 0; i++) {
        putChar(string[i]);
    }
}

void Terminal::setPosition(uint16_t x, uint16_t y) {
    currentColumn = x;
    currentRow = y;
}

void Terminal::setForegroundColor(Color &color) {
    fgColor = color;
}

void Terminal::setBackgroundColor(Color &color) {
    bgColor = color;
}

}