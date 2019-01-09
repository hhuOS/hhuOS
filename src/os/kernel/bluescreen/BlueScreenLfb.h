#ifndef HHUOS_BLUESCREENLFBTEXT_H
#define HHUOS_BLUESCREENLFBTEXT_H

#include <devices/graphics/text/LfbText.h>
#include <lib/multiboot/Structure.h>
#include "BlueScreen.h"

class BlueScreenLfb : public BlueScreen {

public:

    static Multiboot::FrameBufferInfo fbInfo;

public:

    BlueScreenLfb();

    BlueScreenLfb(const BlueScreenLfb &copy) = delete;

    ~BlueScreenLfb() override = default;

    void initialize() override;

private:

    void show(uint16_t x, uint16_t y, char c) override;

private:

    LinearFrameBuffer lfb;

    Font &font = std_font_8x16;

    uint8_t charWidth = font.get_char_width();
    uint8_t charHeight = font.get_char_height();

};

#endif
