#ifndef HHUOS_BLUESCREENCGA_H
#define HHUOS_BLUESCREENCGA_H

#include "BlueScreen.h"

class BlueScreenCga : public BlueScreen {

public:

    BlueScreenCga();

    BlueScreenCga(const BlueScreen &other) = delete;

    BlueScreenCga &operator=(const BlueScreenCga &other) = delete;

    void initialize() override;

private:

    void show(uint16_t x, uint16_t y, char c) override;

private:

    const uint8_t ROWS = 25;

    const uint8_t COLUMNS = 80;

    const uint8_t ATTRIBUTE = 0x1F;

    const char *CGA_START = reinterpret_cast<const char *>(VIRT_CGA_START);

};

#endif
