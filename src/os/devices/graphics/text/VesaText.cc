#include <devices/Pit.h>
#include "VesaText.h"

VesaText::VesaText() : VesaGraphics(), font(&std_font_8x16) {

}

bool VesaText::isAvailable() {
    return VesaGraphics::isAvailable();
}

bool VesaText::setResolution(TextResolution resolution){
    uint8_t fontWidth = font->get_char_width();
    uint8_t fontHeight = font->get_char_height();

    if(shadowBuf != nullptr) {
        delete[] shadowBuf;
        shadowBuf = nullptr;
    }

    if(VesaGraphics::init(resolution.columns * fontWidth, resolution.rows * fontHeight, resolution.depth)) {
        this->columns = VesaGraphics::xres / fontWidth;
        this->rows = VesaGraphics::yres / fontHeight;
        this->depth = VesaGraphics::bpp;

        auto bpp = static_cast<uint8_t>(this->depth == 15 ? 16 : this->depth);
        auto bufSize = static_cast<uint32_t>(VesaGraphics::xres * VesaGraphics::yres * (bpp / 8));

        shadowBuf = new uint8_t[bufSize];
        memset(shadowBuf, 0, bufSize);

        return true;
    }

    return false;
}

Util::ArrayList<TextDriver::TextResolution>& VesaText::getTextResolutions() {
    if(!resolutions.isEmpty()) {
        return resolutions;
    }

    Util::ArrayList<LinearFrameBuffer::LfbResolution>& lfbResolutions = VesaGraphics::getLfbResolutions();

    uint16_t fontWidth = font->get_char_width();
    uint16_t fontHeight = font->get_char_height();

    for(uint32_t i = 0; i < lfbResolutions.size(); i++) {
        LinearFrameBuffer::LfbResolution currentLfbRes = lfbResolutions.get(i);

        auto *currentRes = new TextResolution();
        *currentRes = {static_cast<uint16_t >(currentLfbRes.resX / fontWidth), static_cast<uint16_t >(currentLfbRes.resY / fontHeight), currentLfbRes.depth, currentLfbRes.modeNumber};
        resolutions.add(*currentRes);
    }

    return resolutions;
}

String VesaText::getVendorName() {
    return VesaGraphics::getVendorName();
}

String VesaText::getDeviceName() {
    return VesaGraphics::getDeviceName();
}

uint32_t VesaText::getVideoMemorySize() {
    return VesaGraphics::getVideoMemorySize();
}

void VesaText::drawPixel(uint16_t x, uint16_t y, Color color) {
    //Pixels outside the visible area won't be drawn
    if(x > xres || y > yres) return;

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);

    if(color.getAlpha() < 255) {
        if(color.getAlpha() == 0) {
            return;
        }

        Color currentColor;
        VesaGraphics::readPixel(x, y, currentColor);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(this->bpp);

    //Calculate pixel offset
    uint8_t *ptr = VesaGraphics::hardwareBuffer + (x * (bpp / 8)) + y * pitch;

    //Write color to pixel offset
    for(uint32_t i = 0; i < (bpp / 8); i++) {
        ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
    }

    if(shadowBuf != nullptr) {
        //Calculate pixel offset
        ptr = shadowBuf + (x * (bpp / 8)) + y * pitch;

        //Write color to pixel offset
        for(uint32_t i = 0; i < (bpp / 8); i++) {
            ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
        }
    }
}

void VesaText::setpos(uint16_t x, uint16_t y) {
    pos = x + columns * y;
}

void VesaText::getpos (uint16_t &x, uint16_t &y) {
    x = pos % columns;
    y = pos / columns;
}

void VesaText::show(uint16_t x, uint16_t y, char c, Color fgColor, Color bgColor) {
    uint16_t fontWidth = font->get_char_width();
    uint16_t fontHeight = font->get_char_height();

    VesaText::drawChar(*font, x * fontWidth, y * fontHeight, c, fgColor, bgColor);
}

void VesaText::putc(const char  c, Color fgColor, Color bgColor) {
    puts(&c, 1, fgColor, bgColor);
}

void VesaText::puts(const char  *s, uint32_t n, Color fgColor, Color bgColor) {
    bool cursorState = Pit::getInstance()->isCursorEnabled();
    if (cursorState) {
        Pit::getInstance()->setCursor(false);
    }

    uint16_t x, y;
    getpos(x, y);
    uint16_t pos = x + columns * y;

    for (uint32_t i = 0; i < n; i++) {

        if (s[i] == '\0') {
            break;
        }

        if(s[i] == '\n') {
            show(pos % columns, pos / columns, ' ', fgColor, bgColor);
            pos += columns - (pos % columns);
        } else {
            show(pos % columns, pos / columns, s[i], fgColor, bgColor);
            pos++;
        }

        if(pos >= columns * rows) {
            scrollup();
            pos = rows * columns - columns;
        }
    }

    setpos(pos % columns, pos / columns);

    if(cursorState) {
        Pit::getInstance()->setCursor(true);
    }
}

void VesaText::scrollup () {
    bool cursorState = Pit::getInstance()->isCursorEnabled();
    if (cursorState) {
        Pit::getInstance()->setCursor(false);
    }

    uint16_t fontWidth = font->get_char_width();
    uint16_t fontHeight = font->get_char_height();

    auto bpp = static_cast<uint8_t>(VesaText::bpp == 15 ? 16 : VesaText::bpp);
    uint64_t *src, *dest;
    uint64_t end;

    // Move screen-buffer upwards by one line
    src = (uint64_t *) (shadowBuf + (columns * fontWidth) * fontHeight * (bpp / 8));
    end = ((columns * fontWidth) * (rows * fontHeight - fontHeight) * (bpp / 8)) / sizeof(uint64_t);
    dest = (uint64_t *) shadowBuf;

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = src[i];
    }

    // Clear the last line
    dest = (uint64_t *) (shadowBuf + (columns * fontWidth) * (rows * fontHeight - fontHeight) * (bpp / 8));
    end = (columns * fontWidth) * fontHeight * (bpp / 8) / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = 0;
    }

    // Write buffer to screen memory
    src = (uint64_t *) shadowBuf;
    dest = (uint64_t *) VesaGraphics::hardwareBuffer;
    end = VesaGraphics::xres * VesaGraphics::yres * (bpp / 8) / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = src[i];
    }

    if(cursorState) {
        Pit::getInstance()->setCursor(true);
    }
}

void VesaText::clear () {
    VesaGraphics::clear();
    setpos(0,0);
}