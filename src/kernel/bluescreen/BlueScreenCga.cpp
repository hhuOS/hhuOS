#include "device/misc/Bios.h"
#include "lib/libc/printf.h"
#include "BlueScreenCga.h"

namespace Kernel {

BlueScreenCga::BlueScreenCga() : BlueScreen(80, 25) {

}

void BlueScreenCga::initialize() {
    BC_params->AX = 0x03;
    Bios::Int(0x10);

    BC_params->AX = 0x0100;
    BC_params->CX = 0x2607;
    Bios::Int(0x10);

    auto *dest = (uint64_t *) VIRT_CGA_START;

    uint64_t end = 80 * 25 * 2 / sizeof(uint64_t);

    for (uint64_t i = 0; i < end; i++) {
        dest[i] = 0x1000100010001000;
    }
}

void BlueScreenCga::show(uint16_t x, uint16_t y, const char c) {

    if (x < 0 || x >= COLUMNS || y < 0 || y > ROWS)
        return;

    uint16_t pos = (y * COLUMNS + x) * (uint16_t) 2;

    *((uint8_t *) (CGA_START + pos)) = static_cast<uint8_t>(c);
    *((uint8_t *) (CGA_START + pos + 1)) = ATTRIBUTE;
}

}