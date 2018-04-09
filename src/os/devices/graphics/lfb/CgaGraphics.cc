#include <kernel/Bios.h>
#include "CgaGraphics.h"
#include "kernel/memory/MemLayout.h"

CgaGraphics::CgaGraphics() : LinearFrameBuffer() {

}

bool CgaGraphics::isAvailable() {
    BC_params->AX = 0x1a << 8;
    Bios::Int(0x10);
    auto biosRet = static_cast<uint8_t>(BC_params->BX & 0xff);

    return !(biosRet < 2 || biosRet == 0xff);

}

void CgaGraphics::setMode(uint16_t modeNumber) {
    BC_params->AX = modeNumber;
    Bios::Int(0x10);
}

bool CgaGraphics::setResolution(LinearFrameBuffer::LfbResolution resolution) {
    setMode(resolution.modeNumber);

    hardwareBuffer = (uint8_t *) VIRT_CGA_START;

    return true;
}

void CgaGraphics::reallocBuffer() {
    if(doubleBuffer != nullptr) {
        delete[] doubleBuffer;
        doubleBuffer = nullptr;
    }

    doubleBuffer = new uint8_t[16384];
}

Util::Array<LinearFrameBuffer::LfbResolution> CgaGraphics::getLfbResolutions() {
    if(!resolutions.isEmpty()) {
        return resolutions.toArray();
    }

    auto *currentRes = new LfbResolution();
    *currentRes = {320, 200, 2, 0x04};
    resolutions.add(*currentRes);

    currentRes = new LfbResolution();
    *currentRes = {640, 200, 1, 0x06};
    resolutions.add(*currentRes);

    return resolutions.toArray();
}

String CgaGraphics::getVendorName() {
    return vendorName;
}

String CgaGraphics::getDeviceName() {
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

uint32_t CgaGraphics::getVideoMemorySize() {
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

void CgaGraphics::drawPixel(uint16_t x, uint16_t y, Color color) {
    //Pixels outside the visible area won't be drawn
    if(x > xres - 1 || y > yres - 1) {
        return;
    }

    if(color.getAlpha() < 255) {
        if(color.getAlpha() == 0) {
            return;
        }

        Color currentColor;
        readPixel(x, y, currentColor);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(bpp);

    uint8_t *base = isDoubleBuffered ? doubleBuffer : hardwareBuffer;

    //Calculate pixel offset
    uint8_t *ptr = base + (x / (8 / bpp)) + (y / (4 / bpp)) * pitch;
    if(y % 2 == 1)
        ptr += 0x2000;

    if(bpp == 2) {
        *ptr &= (0xc0 >> ((x % 4) * 2)) ^ 0xff;
        *ptr |= rgbColor << ((3 - (x % 4))) * 2;
    } else {
        *ptr &= (0x80 >> (x % 8)) ^ 0xff;
        *ptr |= rgbColor << ((7 - (x % 8)));
    }
}

void CgaGraphics::readPixel(uint16_t x, uint16_t y, Color &color) {
    if(x > xres - 1 || y > yres - 1) {
        return;
    }

    uint8_t *base = isDoubleBuffered ? doubleBuffer : hardwareBuffer;

    //Calculate pixel offset
    uint8_t *ptr = base + (x / (8 / bpp)) + (y / (4 / bpp)) * pitch;
    if(y % 2 == 1)
        ptr += 0x2000;

    uint8_t ret = 0;
    if(bpp == 2) {
        ret = static_cast<uint8_t>(*ptr & (0xc0 >> ((x % 4)) * 2));
        ret = ret >> ((3 - (x % 4)) * 2);
    } else {
        ret = static_cast<uint8_t>(*ptr & (0x80 >> (x % 8)));
        ret = static_cast<uint8_t>(ret ? 1 : 0);
    }

    color.setRGB(ret, bpp);
}

void CgaGraphics::clear() {
    auto *buf = reinterpret_cast<uint64_t *>(isDoubleBuffered ? doubleBuffer : hardwareBuffer);
    uint64_t end = 16384 / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        buf[i] = 0;
    }
}

void CgaGraphics::enableDoubleBuffering() {
    reallocBuffer();

    isDoubleBuffered = true;
    clear();
}

void CgaGraphics::disableDoubleBuffering() {
    isDoubleBuffered = false;
}

void CgaGraphics::show() {
    if (!isDoubleBuffered) {
        return;
    }

    uint32_t i;
    auto *dest = reinterpret_cast<uint64_t *>(hardwareBuffer);
    auto *src =  reinterpret_cast<uint64_t *>(doubleBuffer);
    uint64_t end = 16384 / sizeof(uint64_t);

    for (i = 0; i < end; i++) {
        dest[i] = src[i];
        src[i] = 0;
    }
}