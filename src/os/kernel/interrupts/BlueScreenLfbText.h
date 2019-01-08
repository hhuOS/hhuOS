#ifndef HHUOS_BLUESCREENLFBTEXT_H
#define HHUOS_BLUESCREENLFBTEXT_H

#include <devices/graphics/text/LfbText.h>
#include "BlueScreen.h"

class BlueScreenLfbText : public BlueScreen {

public:

    static void *LFB_ADDRESS;
    static uint16_t XRES;
    static uint16_t YRES;
    static uint8_t DEPTH;
    static uint16_t PITCH;

public:

    BlueScreenLfbText();

    BlueScreenLfbText(const BlueScreenLfbText &copy) = delete;

    ~BlueScreenLfbText() override = default;

    void initialize() override;

private:

    void show(uint16_t x, uint16_t y, char c) override;

private:

    LfbText lfbText;

};

#endif
