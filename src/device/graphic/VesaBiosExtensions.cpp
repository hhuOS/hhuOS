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
#include "lib/util/base/Constants.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/process/Thread.h"

namespace Device::Graphic {

VesaBiosExtensions::DeviceInfo VesaBiosExtensions::getDeviceInfo() {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();

    // Allocate space for VBE info struct inside lower memory
    auto *vbeInfo = reinterpret_cast<DeviceInfo*>(memoryService.allocateBiosMemory(1));
    auto vbeInfoPhysicalAddress = memoryService.getPhysicalAddress(vbeInfo);
    *vbeInfo = DeviceInfo{};

    // Prepare bios parameters: Store function code in AX and return data address in ES:DI
    Kernel::Thread::Context biosContext{};
    biosContext.eax = BiosFunction::GET_VBE_INFO;
    biosContext.es = static_cast<uint16_t>(reinterpret_cast<uint32_t>(vbeInfoPhysicalAddress) >> 4);

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosContext);
    if (biosReturn.eax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: VesaBiosExtensions Bios Extensions are not supported!");
    }

    // Create a copy of the VBE info struct and free the allocated space in lower memory
    DeviceInfo ret = *vbeInfo;
    delete vbeInfo;

    return ret;
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
    if (biosReturn.eax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }

    // Create a copy of the VBE mode info struct and free the allocated space in lower memory
    ModeInfo ret = *modeInfo;
    delete modeInfo;

    return ret;
}

Util::Array<VesaBiosExtensions::UsableMode> VesaBiosExtensions::getAvailableModes() {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto usableModes = Util::ArrayList<UsableMode>();

    // Get device info and map mode array into virtual memory
    auto deviceInfo = getDeviceInfo();
    auto *modeArrayPhysicalAddress = reinterpret_cast<uint16_t*>((deviceInfo.videoModes[1] << 4) + deviceInfo.videoModes[0]);
    auto modeArrayPageOffset = reinterpret_cast<uint32_t>(modeArrayPhysicalAddress) % Util::PAGESIZE;
    auto *modeArrayPage = memoryService.mapIO(modeArrayPhysicalAddress, 1);
    auto *modeArray = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(modeArrayPage) + modeArrayPageOffset);

    for (uint32_t i = 0; modeArray[i] != MODE_LIST_END_MARKER; i++) {
        auto mode = getModeInfo(modeArray[i]);
        if (mode.physicalAddress == 0 || mode.bpp < 15 || !(mode.attributes & MODE_ATTRIBUTES_HARDWARE_SUPPORT_BIT) || !(mode.attributes & MODE_ATTRIBUTES_LFB_BIT) || (mode.memoryModel != PACKED_PIXEL && mode.memoryModel != DIRECT_COLOR)) {
            continue;
        }

        usableModes.add(UsableMode{mode.resX, mode.resY, mode.bpp, mode.pitch, modeArray[i]});
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
    if (biosReturn.eax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }
}

bool VesaBiosExtensions::DeviceInfo::isValid() {
    return signature[0] == 'V' && signature[1] == 'E' && signature[2] == 'S' && signature[3] == 'A';
}

bool VesaBiosExtensions::UsableMode::operator!=(const VesaBiosExtensions::UsableMode &other) const {
    return resolutionX != other.resolutionX || resolutionY != other.resolutionY || pitch != other.pitch || colorDepth != other.colorDepth || modeNumber != other.modeNumber;
}

}