#include <util/memory/Address.h>
#include "Terminal.h"

namespace Util::Graphic {

Terminal::Terminal(LinearFrameBuffer &lfb, Font &font, char cursor) : columns(lfb.getResolutionX() / font.getCharWidth()), rows(lfb.getResolutionY() / font.getCharHeight()),
        cursor(cursor), lfb(lfb), pixelDrawer(lfb), stringDrawer(pixelDrawer), font(font) {
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
        scrollUp();
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

void Terminal::scrollUp() {
    // Move screen buffer upwards by the given amount of rows
    auto source = Util::Memory::Address<uint32_t>(lfb.getBuffer() + lfb.getPitch() * font.getCharHeight());
    auto destination = Util::Memory::Address<uint32_t>(lfb.getBuffer());
    destination.copyRange(source, lfb.getPitch() * lfb.getResolutionY() - lfb.getPitch() * font.getCharHeight());

    // Clear lower part of the screen
    auto clear = Util::Memory::Address<uint32_t>(lfb.getBuffer() + lfb.getPitch() * (lfb.getResolutionY() - font.getCharHeight()));
    clear.setRange(0, lfb.getPitch() * font.getCharHeight());
}

}