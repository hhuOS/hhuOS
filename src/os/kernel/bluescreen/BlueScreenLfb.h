#ifndef HHUOS_BLUESCREENLFBTEXT_H
#define HHUOS_BLUESCREENLFBTEXT_H

#include <devices/graphics/text/LfbText.h>
#include <lib/multiboot/Structure.h>
#include "BlueScreen.h"

/**
 * Implementation of a bluescreen for a lfb-compatible graphics mode.
 */
class BlueScreenLfb : public BlueScreen {

public:

    static Multiboot::FrameBufferInfo fbInfo;

public:

    /**
     * Constructor.
     */
    BlueScreenLfb();

    /**
     * Copy-constructor.
     */
    BlueScreenLfb(const BlueScreenLfb &copy) = delete;

    /**
     * Assignment operator.
     */
    BlueScreenLfb &operator=(const BlueScreenLfb &other) = delete;

    /**
     * Destructor.
     */
    ~BlueScreenLfb() override = default;

    /**
     * Overriding function from BlueScreen.
     */
    void initialize() override;

private:

    /**
     * Overriding function from BlueScreen.
     */
    void show(uint16_t x, uint16_t y, char c) override;

private:

    LinearFrameBuffer lfb;

    Font &font = std_font_8x16;

    uint8_t charWidth = font.get_char_width();
    uint8_t charHeight = font.get_char_height();

};

#endif
