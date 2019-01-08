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

VesaGraphics::VesaGraphics() : LinearFrameBuffer(nullptr, 0, 0, 0, 0) {
    VbeInfo *vbeInfo = getVbeInfo();

    if(vbeInfo == nullptr) {
        return;
    }

    if (vbeInfo->signature[0]!='V' || vbeInfo->signature[1]!='E' ||
        vbeInfo->signature[2]!='S' || vbeInfo->signature[3]!='A' ) {
        return;
    }

    available = true;

    // Get vendor name
    const char *vendorAddress = (char*) PHYS2VIRT((vbeInfo->vendor[1]<<4) + vbeInfo->vendor[0]);

    if(vendorAddress == nullptr) {
        vendorName = "Unknown";
    } else {
        vendorName = vendorAddress;

        if(vendorName.isEmpty()) {
            vendorName = "Unknown";
        }
    }

    // Get video memory size
    videoMemorySize = ((uint32_t) vbeInfo->video_memory) * 65536;

    // Get device name
    const char *deviceNameAddress = (char*) PHYS2VIRT((vbeInfo->product_name[1]<<4) + vbeInfo->product_name[0]);

    if(deviceNameAddress == nullptr) {
        deviceName = "Unknown";
    } else {
        deviceName = deviceNameAddress;

        if(deviceName.isEmpty()) {
            deviceName = "Unknown";
        }
    }

    // Get available resolutions
    auto *modePtr = (uint16_t*) (((vbeInfo->video_modes[1] << 4) + vbeInfo->video_modes[0]) + KERNEL_START);
    uint16_t modes[256];

    uint32_t i;
    for (i = 0; modePtr[i] != INVALID_MODE; i++) {
        modes[i] = modePtr[i];
    }
    modes[i] = INVALID_MODE;

    for (i = 0; modes[i] != INVALID_MODE; i++) {
        ModeInfo *modeInfo = getModeInfo(modes[i]);

        if (modeInfo == nullptr || modeInfo->physbase == 0 || modeInfo->bpp < 15 || !(modeInfo->attributes & 90) ||
            (modeInfo->memory_model != 4 && modeInfo->memory_model != 6)) {
            continue;
        }

        resolutions.add({modeInfo->Xres, modeInfo->Yres, modeInfo->bpp, modes[i]});
    }
}

String VesaGraphics::getName() {
    return NAME;
}

bool VesaGraphics::setMode(uint16_t mode) {
	BC_params->AX = 0x4F02;
	BC_params->BX = mode;
	Bios::Int(0x10);

    return BC_params->AX == 0x004F;
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
    return available && (getLfbResolutions().length() != 0);
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
    return resolutions.toArray();
}

String VesaGraphics::getVendorName() {
    return vendorName;
}

String VesaGraphics::getDeviceName() {
    return deviceName;
}

uint32_t VesaGraphics::getVideoMemorySize() {
    return videoMemorySize;
}
