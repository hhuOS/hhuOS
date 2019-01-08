#include "BlueScreenLfbText.h"

void *BlueScreenLfbText::LFB_ADDRESS = nullptr;

uint16_t BlueScreenLfbText::XRES = 0;

uint16_t BlueScreenLfbText::YRES = 0;

uint8_t BlueScreenLfbText::DEPTH = 0;

uint16_t BlueScreenLfbText::PITCH = 0;

BlueScreenLfbText::BlueScreenLfbText() : BlueScreen(XRES, YRES), lfbText(LFB_ADDRESS, XRES, YRES, DEPTH, PITCH) {

}

void BlueScreenLfbText::initialize() {
    TextDriver &text = lfbText;

    auto columns = static_cast<uint16_t>(XRES / 8);
    auto rows = static_cast<uint16_t>(YRES / 16);

    text.init(columns, rows, DEPTH);

    for(uint16_t i = 0; i < columns + 1; i++) {
        for(uint16_t j = 0; j < rows + 1; j++) {
            lfbText.show(i, j, ' ', Colors::BLUE, Colors::BLUE);
        }
    }
}

void BlueScreenLfbText::show(uint16_t x, uint16_t y, char c) {
    lfbText.show(x, y, c, Colors::WHITE, Colors::INVISIBLE);
}
