/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <kernel/Bios.h>
#include <kernel/memory/MemLayout.h>
#include <lib/libc/printf.h>
#include "kernel/memory/SystemManagement.h"
#include "VesaGraphics.h"

VesaGraphics::VesaGraphics() : LinearFrameBuffer() {

}

bool VesaGraphics::setMode(uint16_t mode) {
	BC_params->AX = 0x4F02;
	BC_params->BX = mode;
	Bios::Int(0x10);

	if (BC_params->AX != 0x004F) {
		return false;
	}

	return true;
}

VesaGraphics::VbeInfo* VesaGraphics::getVbeInfo() {
    BC_params->AX = 0x4F00;
    BC_params->ES = static_cast<uint16_t >(VIRT_BIOS_RETURN_MEM >> 4);
    BC_params->DI = VIRT_BIOS_RETURN_MEM & 0xF;

    memset((void *) VIRT_BIOS_RETURN_MEM, 0, sizeof(VbeInfo));
    Bios::Int(0x10);

    if (BC_params->AX != 0x004F) {
        return nullptr;
    }

	return (VesaGraphics::VbeInfo*) VIRT_BIOS_RETURN_MEM;
}

VesaGraphics::ModeInfo* VesaGraphics::getModeInfo(uint16_t mode) {
	BC_params->AX = 0x4F01;
	BC_params->CX = mode;
	BC_params->ES = static_cast<uint16_t>(VIRT_BIOS_RETURN_MEM >> 4);
	BC_params->DI = VIRT_BIOS_RETURN_MEM & 0xF;

    memset((void *) VIRT_BIOS_RETURN_MEM, 0, sizeof(ModeInfo));
	Bios::Int(0x10);

	if (BC_params->AX != 0x004F) {
		return nullptr;
	}

	return (ModeInfo*) VIRT_BIOS_RETURN_MEM;
}

bool VesaGraphics::isAvailable() {
    VbeInfo *vbeInfo = getVbeInfo();

    if(vbeInfo == nullptr)
        return false;
    
    if (vbeInfo->signature[0]!='V' || vbeInfo->signature[1]!='E' ||
        vbeInfo->signature[2]!='S' || vbeInfo->signature[3]!='A' )
            return false;

    return getLfbResolutions().length() != 0;
}


bool VesaGraphics::setResolution(LfbResolution resolution) {
    ModeInfo *modeInfo = getModeInfo(resolution.modeNumber);

    if(modeInfo == nullptr)
        return false;

    void *tmpAddress = SystemManagement::getInstance().mapIO(modeInfo->physbase,
            static_cast<uint32_t>(modeInfo->Xres * modeInfo->Yres * (modeInfo->bpp == 15 ? 16 : modeInfo->bpp) / 8));

    setMode(resolution.modeNumber);

    pitch = modeInfo->pitch;

    hardwareBuffer = reinterpret_cast<uint8_t *>(tmpAddress);

    doubleBuffered = false;

    if(doubleBuffer != nullptr) {
        delete[] doubleBuffer;
        doubleBuffer = nullptr;
    }

    if(virtLfbAddress != nullptr) {
    	SystemManagement::getInstance().freeIO(virtLfbAddress);
    }

    virtLfbAddress = tmpAddress;

    return true;
}

Util::Array<LinearFrameBuffer::LfbResolution> VesaGraphics::getLfbResolutions() {
    if(!resolutions.isEmpty()) {
        return resolutions.toArray();
    }

    VbeInfo *vbeInfo = getVbeInfo();

    if(vbeInfo == nullptr) {
        return resolutions.toArray();
    }

    if (vbeInfo->signature[0]!='V' || vbeInfo->signature[1]!='E' ||
        vbeInfo->signature[2]!='S' || vbeInfo->signature[3]!='A' ) {
        return resolutions.toArray();
    }

    auto *modePtr = (uint16_t*) (((vbeInfo->video_modes[1] << 4) + vbeInfo->video_modes[0]) + KERNEL_START);
    uint16_t modes[256];

	uint32_t i;
	for (i = 0; modePtr[i] != 0xffff; i++) {
		modes[i] = modePtr[i];
	}
    modes[i] = 0xffff;

    for (i = 0; modes[i] != 0xFFFF; i++) {
        ModeInfo *modeInfo = getModeInfo(modes[i]);

        if (modeInfo == nullptr || modeInfo->physbase == 0 || modeInfo->bpp < 15 || !(modeInfo->attributes & 90) ||
                (modeInfo->memory_model != 4 && modeInfo->memory_model != 6)) {
            continue;
        }

        auto *currentRes = new LfbResolution();
        *currentRes = {modeInfo->Xres, modeInfo->Yres, modeInfo->bpp, modes[i]};
        resolutions.add(*currentRes);
    }

    return resolutions.toArray();
}

String VesaGraphics::getVendorName() {
    if(!vendorName.isEmpty()) {
        return vendorName;
    }

    VbeInfo *vbeInfo = getVbeInfo();

    if(vbeInfo == nullptr)
        return vendorName;

    if (vbeInfo->signature[0]!='V' || vbeInfo->signature[1]!='E' ||
        vbeInfo->signature[2]!='S' || vbeInfo->signature[3]!='A' )
        return vendorName;

    const char *vendorAddress = (char*) PHYS2VIRT((vbeInfo->vendor[1]<<4) + vbeInfo->vendor[0]);

    if(vendorAddress == nullptr) {
        vendorName = "Unknown";
    } else {
        vendorName = vendorAddress;
    }

    return vendorName;
}

String VesaGraphics::getDeviceName() {
    if(!deviceName.isEmpty()) {
        return deviceName;
    }

    VbeInfo *vbeInfo = getVbeInfo();

    if(vbeInfo == nullptr)
        return deviceName;

    if (vbeInfo->signature[0]!='V' || vbeInfo->signature[1]!='E' ||
        vbeInfo->signature[2]!='S' || vbeInfo->signature[3]!='A' )
        return deviceName;

    const char *deviceNameAddress = (char*) PHYS2VIRT((vbeInfo->product_name[1]<<4) + vbeInfo->product_name[0]);

    if(deviceNameAddress == nullptr) {
        deviceName = "Unknown";
    } else {
        deviceName = deviceNameAddress;
    }

    return deviceName;
}

uint32_t VesaGraphics::getVideoMemorySize() {
    if(videoMemorySize != 0) {
        return videoMemorySize;
    }

    VbeInfo *vbeInfo = getVbeInfo();

    if(vbeInfo == nullptr)
        return videoMemorySize;

    if (vbeInfo->signature[0]!='V' || vbeInfo->signature[1]!='E' ||
        vbeInfo->signature[2]!='S' || vbeInfo->signature[3]!='A' )
        return videoMemorySize;

    videoMemorySize = ((uint32_t) vbeInfo->video_memory) * 65536;

    return videoMemorySize;
}

void VesaGraphics::drawPixel(uint16_t x, uint16_t y, Color color) {
    //Pixels outside the visible area won't be drawn
    if(x > xres - 1 || y > yres - 1) return;
    
    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);
    
    if(color.getAlpha() < 255) {
        if(color.getAlpha() == 0) {
            return;
        }

        Color currentColor;
        readPixel(x, y, currentColor);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(this->bpp);

    uint8_t *base = doubleBuffered ? doubleBuffer : hardwareBuffer;

    //Calculate pixel offset
    uint8_t *ptr = base + (x * (bpp / 8)) + y * pitch;

    //Write color to pixel offset
    for(uint32_t i = 0; i < (bpp / 8); i++) {
        ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
    }
}

void VesaGraphics::readPixel(uint16_t x, uint16_t y, Color &color) {
    if (x > xres - 1 || y > yres - 1) {
        return;
    }

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);
    uint8_t *base = doubleBuffered ? doubleBuffer : hardwareBuffer;
    uint8_t *ptr = base + (x * (bpp / 8)) + y * pitch;

    color.setRGB(*((uint32_t *)ptr), this->bpp);
}

void VesaGraphics::clear() {
    auto *buf = reinterpret_cast<uint64_t *>(doubleBuffered ? doubleBuffer : hardwareBuffer);
    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);
    uint32_t end = (xres * yres * (bpp / 8)) / sizeof(uint64_t);

    for(uint32_t i = 0; i < end; i++) {
        buf[i] = 0;
    }
}

void VesaGraphics::reallocBuffer() {
    if(doubleBuffer != nullptr) {
        delete[] doubleBuffer;
        doubleBuffer = nullptr;
    }

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);
    doubleBuffer = new uint8_t[xres * yres * (bpp / 8)];
}

void VesaGraphics::enableDoubleBuffering() {
    reallocBuffer();

    doubleBuffered = true;
    clear();
}

void VesaGraphics::disableDoubleBuffering() {
    doubleBuffered = false;
}

bool VesaGraphics::isDoubleBuffered() {
    return doubleBuffered;
}

void VesaGraphics::show() {
    if (!doubleBuffered) {
        return;
    }

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);

    uint32_t i;
    auto *dest = reinterpret_cast<uint64_t *>(hardwareBuffer);
    auto *src =  reinterpret_cast<uint64_t *>(doubleBuffer);
    uint64_t end = (xres * yres * (bpp / 8)) / sizeof(uint64_t);

    for (i = 0; i < end; i++) {
        dest[i] = src[i];
        src[i] = 0;
    }
}
