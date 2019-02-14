#ifndef HHUOS_BLUESCREENCGA_H
#define HHUOS_BLUESCREENCGA_H

#include "BlueScreen.h"

/**
 * Implementation of a bluescreen for the CGA graphics mode.
 *
 * CAUTION: This class will use a BIOS-call to change the graphics mode!
 */
class BlueScreenCga : public BlueScreen {

public:

    /**
     * Constructor.
     */
    BlueScreenCga();

    /**
     * Copy-constructor.
     */
    BlueScreenCga(const BlueScreenCga &other) = delete;

    /**
     * Assignment operator.
     */
    BlueScreenCga &operator=(const BlueScreenCga &other) = delete;

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

    const uint8_t ROWS = 25;

    const uint8_t COLUMNS = 80;

    const uint8_t ATTRIBUTE = 0x1F;

    const char *CGA_START = reinterpret_cast<const char *>(VIRT_CGA_START);

};

#endif
