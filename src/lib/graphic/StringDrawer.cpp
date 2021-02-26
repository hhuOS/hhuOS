#include "StringDrawer.h"

Util::StringDrawer::StringDrawer(Util::PixelDrawer &pixelDrawer): pixelDrawer(pixelDrawer) {}

void Util::StringDrawer::drawChar(Util::Font &font, uint16_t x, uint16_t y, char c, Util::Color &fgColor, Util::Color &bgColor) {
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(c));
}

void Util::StringDrawer::drawString(Util::Font &font, uint16_t x, uint16_t y, const char *string, Util::Color &fgColor, Util::Color &bgColor) {
    for(uint32_t i = 0; string[i] != 0; ++i) {
        drawChar(font, x, y, string[i], fgColor, bgColor);
        x += font.getCharWidth();
    }
}

void Util::StringDrawer::drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Util::Color &fgColor, Util::Color &bgColor, uint8_t *bitmap) {
    auto widthInBytes = static_cast<uint16_t>(width / 8 + ((width % 8 != 0) ? 1 : 0));

    for(uint16_t offsetY = 0; offsetY < height; ++offsetY) {
        uint16_t posX = x;
        uint16_t posY = y + offsetY;

        for(uint16_t xb = 0; xb < widthInBytes; ++xb) {
            for(int8_t src = 7; src >= 0; --src) {
                if((1 << src) & *bitmap)
                    pixelDrawer.drawPixel(posX, posY, fgColor);
                else
                    pixelDrawer.drawPixel(posX, posY, bgColor);
                posX++;
            }
            bitmap++;
        }
    }
}
