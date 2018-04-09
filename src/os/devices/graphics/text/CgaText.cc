/*****************************************************************************
 *                                                                           *
 *                                    C G A                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Mit Hilfe dieser Klasse kann man auf den Bildschirm des  *
 *                  PCs zugreifen. Der Zugriff erfolgt direkt auf der Hard-  *
 *                  wareebene, d.h. ueber den Bildschirmspeicher und den     *
 *                  I/O-Ports der Grafikkarte.                               *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Aenderungen von Michael Schoettner, HHU, 2.2.2017        *
 *****************************************************************************/
#include <kernel/Bios.h>
#include "CgaText.h"

CgaText::CgaText() : TextDriver(), index_port(0x3d4), data_port(0x3d5) {
    columns = 80;
    rows = 25;
    depth = 4;
}

bool CgaText::isAvailable() {
    BC_params->AX = 0x1a << 8;
    Bios::Int(0x10);
    uint32_t biosRet = BC_params->BX & 0xff;

    return !(biosRet < 2 || biosRet == 0xff);
}

Util::Array<TextDriver::TextResolution> CgaText::getTextResolutions() {
    if(!resolutions.isEmpty()) {
        return resolutions.toArray();
    }

    auto *currentRes = new TextResolution();
    *currentRes = {40, 25, 4, 0x01};
    resolutions.add(*currentRes);

    currentRes = new TextResolution();
    *currentRes = {80, 25, 4, 0x03};
    resolutions.add(*currentRes);

    return resolutions.toArray();
}

void CgaText::setMode(uint16_t modeNumber) {
    BC_params->AX = modeNumber;
    Bios::Int(0x10);

    BC_params->AX = 0x0100;
    BC_params->CX = 0x2607;
    Bios::Int(0x10);
}

bool CgaText::setResolution(TextResolution resolution) {
    setMode(resolution.modeNumber);

    return true;
}

String CgaText::getVendorName() {
    return vendorName;
}

String CgaText::getDeviceName() {
    if(!deviceName.isEmpty()) {
        return deviceName;
    }

    BC_params->AX = 0x1a << 8;
    Bios::Int(0x10);
    uint32_t biosRet = BC_params->BX & 0xff;

    switch(biosRet) {
        case 0x01:
            deviceName = "Generic MDA";
            break;
        case 0x02:
            deviceName = "Generic CGA";
            break;
        case 0x04:
            deviceName = "Generic EGA";
            break;
        case 0x05:
            deviceName = "Generic EGA";
            break;
        case 0x07:
            deviceName = "Generic VGA";
            break;
        case 0x08:
            deviceName = "Generic VGA";
            break;
        case 0x0a:
            deviceName = "Generic MCGA";
            break;
        case 0x0b:
            deviceName = "Generic MCGA";
            break;
        case 0x0c:
            deviceName = "Generic MCGA";
            break;
        default:
            deviceName = "Unknown";
            break;
    }

    return deviceName;
}

uint32_t CgaText::getVideoMemorySize() {
    if(videoMemorySize != 0) {
        return videoMemorySize;
    }

    BC_params->AX = 0x1a << 8;
    Bios::Int(0x10);
    uint32_t biosRet = BC_params->BX & 0xff;

    switch(biosRet) {
        case 0x01:
            videoMemorySize = 4096;
            break;
        case 0x02:
            videoMemorySize = 16384;
            break;
        case 0x04:
            videoMemorySize = 131072;
            break;
        case 0x05:
            videoMemorySize = 131072;
            break;
        case 0x07:
            videoMemorySize = 262144;
            break;
        case 0x08:
            videoMemorySize = 262144;
            break;
        case 0x0a:
            videoMemorySize = 65536;
            break;
        case 0x0b:
            videoMemorySize = 65536;
            break;
        case 0x0c:
            videoMemorySize = 65536;
            break;
        default:
            videoMemorySize = 0;
            break;
    }

    return videoMemorySize;
}

void CgaText::setpos(uint16_t x, uint16_t y) {
    if (x < 0 || x >= columns || y < 0 || y > rows)
        return ;

    pos = y * columns + x;

    // Set position of hardware cursor
    auto low  = static_cast<uint8_t>(pos & 0xff);
    auto high = static_cast<uint8_t>((pos & 0x3f00) >> 8);

    // Wirte high byte
    index_port.outb(14);
    data_port.outb(high);

    // Write low byte
    index_port.outb(15);
    data_port.outb(low);

}

void CgaText::getpos (uint16_t &x, uint16_t &y) {
    x = pos % columns;
    y = pos / columns;
}

void CgaText::show(uint16_t x, uint16_t y, const char c, Color fgColor, Color bgColor) {
    if (x < 0 || x >= columns || y < 0 || y > rows)
        return;

    uint16_t pos = (y * columns + x) * (uint16_t) 2;

    if(bgColor.getAlpha() < 255) {
        uint8_t currentAttrib = *((uint8_t*) (CGA_START + pos + 1)) >> 4;
        Color currentColor(currentAttrib, depth);

        if(bgColor.getAlpha() > 0) {
            currentColor.blendWith(bgColor);
            bgColor = currentColor;
        } else {
            bgColor = currentColor;
        }
    }

    if(fgColor.getAlpha() < 255 && fgColor.getAlpha() > 0) {
        auto currentAttrib = static_cast<uint8_t>(*((uint8_t*) (CGA_START + pos + 1)) & 0x0f);
        Color currentColor(currentAttrib, depth);

        if(fgColor.getAlpha() > 0) {
            currentColor.blendWith(fgColor);
            fgColor = currentColor;
        } else {
            fgColor = currentColor;
        }
    }

    uint8_t attrib = (bgColor.getRGB4() << 4) | (fgColor.getRGB4());

    *((uint8_t *) (CGA_START + pos)) = static_cast<uint8_t>(c);
    *((uint8_t *) (CGA_START + pos + 1)) = attrib;
}

void CgaText::putc(const char c, Color fgColor, Color bgColor) {
    uint16_t x, y;

    getpos(x, y);

    if(c == '\n') {
        x = 0;
        y++;
    } else {
        show(x, y, c, fgColor, bgColor);
        x++;
    }

    if(x >= columns) {
        x = 0;
        y++;
    }
    
    if(y >= rows) {
        scrollup();
        x = 0;
        y = static_cast<uint16_t>(rows - 1);
    }
    
    setpos(x, y);
}

void CgaText::puts(const char *s, uint32_t n, Color fgColor, Color bgColor) {
    for(uint32_t i = 0; i < n; i++) {
        putc(s[i], fgColor, bgColor);
    }
}

void CgaText::scrollup () {
    // Move screen upwards by one line
    auto *src = (uint64_t *) (CGA_START + columns * 2);
    auto *dest = (uint64_t *) CGA_START;
    uint64_t end = (rows - 1) * columns * 2 / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = src[i];
    }

    // Clear the last line
    dest = (uint64_t *) (CGA_START + ((rows - 1) * columns * 2));
    end = (columns * 2) / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = 0x0f20;
    }
 }

void CgaText::clear () {
    auto *dest = (uint64_t *) CGA_START;
    uint64_t end = rows * columns * 2 / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = 0;
    }

    setpos(0, 0);
}


