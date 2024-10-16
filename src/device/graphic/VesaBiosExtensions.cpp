/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include "VesaBiosExtensions.h"

#include "kernel/service/MemoryService.h"
#include "device/system/Bios.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/process/Thread.h"
#include "kernel/log/Log.h"
#include "lib/util/math/Math.h"
#include "kernel/memory/MemoryLayout.h"

namespace Device::Graphic {

VesaBiosExtensions::VesaBiosExtensions(const DeviceInfo *deviceInfo, const Util::Array<UsableMode> &supportedModes) : deviceInfo(*deviceInfo), supportedModes(supportedModes) {}

VesaBiosExtensions::~VesaBiosExtensions() {
    delete &deviceInfo;
}

VesaBiosExtensions* VesaBiosExtensions::initialize() {
    if (!Bios::isAvailable()) {
        return nullptr;
    }

    auto *deviceInfo = getVbeInfo();
    if (deviceInfo == nullptr || !deviceInfo->isValid()) {
        LOG_ERROR("Failed to get VBE device info from BIOS");
        delete deviceInfo;
        return nullptr;
    }

    if (deviceInfo->vbeVersion < 0x0200) {
        LOG_INFO("VBE [%u.%u] compatible graphics card found (OEM string: [%s])", (deviceInfo->vbeVersion & 0xff00) >> 8, deviceInfo->vbeVersion & 0x00ff, deviceInfo->getOemString());
    } else {
        LOG_INFO("VBE [%u.%u] compatible graphics card found (OEM string: [%s], Vendor: [%s], Device: [%s], Revision: [%s])",
                 (deviceInfo->vbeVersion & 0xff00) >> 8, deviceInfo->vbeVersion & 0x00ff,
                 deviceInfo->getOemString(), deviceInfo->getVendorName(), deviceInfo->getProductName(), deviceInfo->getProductRevision());
    }

    auto modes = getModes(*deviceInfo);
    return new VesaBiosExtensions(deviceInfo, modes);
}

bool VesaBiosExtensions::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    VesaBiosExtensions::DeviceInfo *deviceInfo = getVbeInfo();
    auto available = (deviceInfo != nullptr && deviceInfo->isValid());

    delete deviceInfo;
    return available;
}

VesaBiosExtensions::DeviceInfo* VesaBiosExtensions::getVbeInfo() {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();

    // Allocate space for VBE info struct inside lower memory
    auto *vbeInfo = reinterpret_cast<DeviceInfo*>(memoryService.allocateBiosMemory(1));
    auto vbeInfoPhysicalAddress = memoryService.getPhysicalAddress(vbeInfo);
    *vbeInfo = DeviceInfo{};
    vbeInfo->signature[0] = 'V';
    vbeInfo->signature[1] = 'B';
    vbeInfo->signature[2] = 'E';
    vbeInfo->signature[3] = '2';

    // Prepare bios parameters: Store function code in AX and return data address in ES:DI
    Kernel::Thread::Context biosContext{};
    biosContext.eax = BiosFunction::GET_VBE_INFO;
    biosContext.es = static_cast<uint16_t>(reinterpret_cast<uint32_t>(vbeInfoPhysicalAddress) >> 4);

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosContext);
    if (!checkReturnStatus(biosReturn)) {
        delete vbeInfo;
        return nullptr;
    }

    return vbeInfo;
}

VesaBiosExtensions::ModeInfo VesaBiosExtensions::getModeInfo(uint16_t mode) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();

    // Allocate space for VBE info struct inside lower memory
    auto *modeInfo = reinterpret_cast<ModeInfo*>(memoryService.allocateBiosMemory(1));
    auto modeInfoPhysicalAddress = memoryService.getPhysicalAddress(modeInfo);
    *modeInfo = ModeInfo{};

    // Prepare bios parameters: Store function code in AX, mode number in CX and return data address in ES:DI
    Kernel::Thread::Context biosContext{};
    biosContext.eax = BiosFunction::GET_MODE_INFO;
    biosContext.ecx = mode;
    biosContext.es = static_cast<uint16_t>(reinterpret_cast<uint32_t>(modeInfoPhysicalAddress) >> 4);

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosContext);
    if (!checkReturnStatus(biosReturn)) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }

    // Create a copy of the VBE mode info struct and free the allocated space in lower memory
    ModeInfo ret = *modeInfo;
    delete modeInfo;

    return ret;
}

const Util::Array<VesaBiosExtensions::UsableMode>& VesaBiosExtensions::getSupportedModes() const {
    return supportedModes;
}

Util::Array<VesaBiosExtensions::UsableMode> VesaBiosExtensions::getModes(const DeviceInfo &deviceInfo) {
    auto usableModes = Util::ArrayList<UsableMode>();
    auto *modeArray = deviceInfo.getVideoModeArray();

    for (uint32_t i = 0; modeArray[i] != MODE_LIST_END_MARKER; i++) {
        auto mode = getModeInfo(modeArray[i]);
        if (mode.physicalAddress == 0 || mode.bpp < 15 || !(mode.attributes & MODE_ATTRIBUTES_HARDWARE_SUPPORT_BIT) || !(mode.attributes & MODE_ATTRIBUTES_LFB_BIT) || (mode.memoryModel != PACKED_PIXEL && mode.memoryModel != DIRECT_COLOR)) {
            continue;
        }

        LOG_DEBUG("Found mode [%ux%u@%u]", mode.resX, mode.resY, mode.bpp);
        usableModes.add(UsableMode{mode.resX, mode.resY, mode.bpp, mode.pitch, mode.physicalAddress, modeArray[i]});
    }

    return usableModes.toArray();
}

void VesaBiosExtensions::setMode(uint16_t mode) {
    // Prepare bios parameters: Store function code in AX and mode number in BX
    Kernel::Thread::Context biosContext{};
    biosContext.eax = BiosFunction::SET_MODE;
    biosContext.ebx = mode | MODE_NUMBER_LFB_BIT;

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosContext);
    if (!checkReturnStatus(biosReturn)) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }
}

const VesaBiosExtensions::DeviceInfo& VesaBiosExtensions::getDeviceInfo() const {
    return deviceInfo;
}

const VesaBiosExtensions::UsableMode& VesaBiosExtensions::findMode(uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth) const {
    uint32_t bestPixelDifference = UINT32_MAX;
    uint8_t bestDepthDifference = UINT8_MAX;
    const auto pixels = resolutionX * resolutionY;
    const auto *bestMode = &supportedModes[0];

    for (const auto &mode : supportedModes) {
        const auto pixelDifference = Util::Math::absolute((mode.resolutionX * mode.resolutionY) - pixels);
        const auto depthDifference = Util::Math::absolute(mode.colorDepth - colorDepth);

        if (pixelDifference < bestPixelDifference || (pixelDifference == bestPixelDifference && depthDifference < bestDepthDifference)) {
            bestMode = &mode;
            bestPixelDifference = pixelDifference;
            bestDepthDifference = depthDifference;
        }
    }

    return *bestMode;
}

bool VesaBiosExtensions::checkReturnStatus(const Kernel::Thread::Context &biosReturnContext) {
    return (biosReturnContext.eax & 0x0000ffff) == BIOS_CALL_RETURN_CODE_SUCCESS;
}

bool VesaBiosExtensions::DeviceInfo::isValid() const {
    return signature[0] == 'V' && signature[1] == 'E' && signature[2] == 'S' && signature[3] == 'A';
}

const char* VesaBiosExtensions::DeviceInfo::getOemString() const {
    return static_cast<const char*>(calculateVirtualAddress(oem[0], oem[1]));
}

const char* VesaBiosExtensions::DeviceInfo::getVendorName() const {
    return static_cast<const char*>(calculateVirtualAddress(vendor[0], vendor[1]));
}

const char* VesaBiosExtensions::DeviceInfo::getProductName() const {
    return static_cast<const char*>(calculateVirtualAddress(productName[0], productName[1]));
}

const char* VesaBiosExtensions::DeviceInfo::getProductRevision() const {
    return static_cast<const char*>(calculateVirtualAddress(productRevision[0], productRevision[1]));
}

const uint16_t* VesaBiosExtensions::DeviceInfo::getVideoModeArray() const {
    return static_cast<const uint16_t*>(calculateVirtualAddress(videoModes[0], videoModes[1]));
}

const void* VesaBiosExtensions::DeviceInfo::calculateVirtualAddress(uint16_t segment, uint16_t offset) const {
    auto *address = reinterpret_cast<const void*>((offset << 4) + segment);

    if (reinterpret_cast<uint32_t>(address) > Kernel::MemoryLayout::USABLE_LOWER_MEMORY.endAddress) {
        // Data is located inside BIOS data area, which is identity mapped
        return address;
    } else {
        // Data is located inside oemData area of this struct -> Physical address needs to be translated to virtual address
        auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
        auto *physicalStructAddress = memoryService.getPhysicalAddress(const_cast<DeviceInfo*>(this));
        auto structOffset = reinterpret_cast<uint32_t>(address) - reinterpret_cast<uint32_t>(physicalStructAddress);

        return reinterpret_cast<const char*>(this) + structOffset;
    }
}

bool VesaBiosExtensions::UsableMode::operator!=(const VesaBiosExtensions::UsableMode &other) const {
    return resolutionX != other.resolutionX || resolutionY != other.resolutionY || pitch != other.pitch || colorDepth != other.colorDepth || modeNumber != other.modeNumber;
}

}