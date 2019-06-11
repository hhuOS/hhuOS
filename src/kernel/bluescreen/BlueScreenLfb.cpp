#include "BlueScreenLfb.h"

namespace Kernel {

Multiboot::FrameBufferInfo BlueScreenLfb::fbInfo{};

BlueScreenLfb::BlueScreenLfb() : BlueScreen(fbInfo.width, fbInfo.height),
                                 lfb(fbInfo.address, fbInfo.width, fbInfo.height, fbInfo.bpp, fbInfo.pitch) {

    columns = static_cast<uint16_t>(fbInfo.width / font.get_char_width());
    rows = static_cast<uint16_t>(fbInfo.height / font.get_char_height());
}

void BlueScreenLfb::initialize() {
    lfb.init(fbInfo.width, fbInfo.height, fbInfo.bpp);

    lfb.fillRect(0, 0, static_cast<uint16_t>(fbInfo.width - 1), static_cast<uint16_t>(fbInfo.height - 1), Colors::BLUE);
}

void BlueScreenLfb::show(uint16_t x, uint16_t y, char c) {
    lfb.drawChar(font, x * font.get_char_width(), y * font.get_char_height(), c, Colors::WHITE, Colors::INVISIBLE);
}

}
