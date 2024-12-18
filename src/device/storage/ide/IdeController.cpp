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
 *
 * The IDE driver is based on a bachelor's thesis, written by Tim Laurischkat.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-tilau101
 *
 * The driver has been enhanced with ATAPI capabilities during a bachelor's thesis, written by Moritz Riefer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-morie103
 */

#include "IdeController.h"

#include "lib/util/async/Thread.h"
#include "device/storage/ChsConverter.h"
#include "IdeDevice.h"
#include "kernel/service/StorageService.h"
#include "kernel/service/InterruptService.h"
#include "device/bus/pci/Pci.h"
#include "device/bus/pci/PciDevice.h"
#include "kernel/log/Log.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/String.h"
#include "lib/util/time/Timestamp.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Iterator.h"
#include "kernel/service/Service.h"
#include "kernel/service/TimeService.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Storage {

IdeController::IdeController(const PciDevice &pciDevice) {
    LOG_INFO("Initializing controller [0x%04x:0x%04x]", pciDevice.getVendorId(), pciDevice.getDeviceId());

    uint32_t baseAddress;
    uint32_t controlBaseAddress;
    uint32_t dmaBaseAddress = 0;

    uint16_t command = pciDevice.readWord(Pci::COMMAND);
    command |= Pci::IO_SPACE;

    if (pciDevice.getProgrammingInterface() & 0x80) {
        LOG_INFO("Controller supports DMA");
        supportsDma = true;
        dmaBaseAddress = pciDevice.readDoubleWord(Pci::Register::BASE_ADDRESS_4) & 0xfffffffc;

        command |= Pci::BUS_MASTER;
    }

    pciDevice.writeWord(Pci::COMMAND, command);

    for (uint32_t i = 0; i < CHANNELS_PER_CONTROLLER; i++) {
        auto channelInterface = pciDevice.getProgrammingInterface() >> i * 2;
        if ((channelInterface & 0x01) == 0x00) {
            // Controller is running in compatibility mode
            if ((channelInterface & 0x02) == 0x02) {
                // Mode can be modified -> Change to native mode
                LOG_INFO("Changing mode of channel [%u] to native mode", i);
                pciDevice.writeByte(Pci::Register::PROGRAMMING_INTERFACE, pciDevice.getProgrammingInterface() | (0x01 << i * 2));
            }
        }

        channelInterface = pciDevice.getProgrammingInterface() >> i * 2;
        if ((channelInterface & 0x01) == 0x00) {
            // Controller is still running in compatibility mode -> Use default base address
            LOG_INFO("Channel [%u] is running in compatibility mode", i);
            baseAddress = DEFAULT_BASE_ADDRESSES[i];
            controlBaseAddress = DEFAULT_CONTROL_BASE_ADDRESSES[i];
        } else {
            // Controller is running in native mode -> Read base address from PCI Registers
            LOG_INFO("Channel [%u] is running in native mode", i);
            baseAddress = pciDevice.readDoubleWord(Pci::Register::BASE_ADDRESS_0) & 0xfffffffc;
            controlBaseAddress = pciDevice.readDoubleWord(Pci::Register::BASE_ADDRESS_1) & 0xfffffffc;
        }

        channels[i] = ChannelRegisters(baseAddress, controlBaseAddress, dmaBaseAddress + (i == 0 ? 0 : BUS_MASTER_CHANNEL_OFFSET));
    }
}

IdeController::CommandRegisters::CommandRegisters(uint16_t baseAddress) :
        data(baseAddress + 0x00), error(baseAddress + 0x01), features(baseAddress + 0x01), sectorCount(baseAddress + 0x02),
        sectorNumber(baseAddress + 0x03), lbaLow(baseAddress + 0x03), cylinderLow(baseAddress + 0x04), lbaMid(baseAddress + 0x04),
        cylinderHigh(baseAddress + 0x05), lbaHigh(baseAddress + 0x05), driveHead(baseAddress + 0x06), status(baseAddress + 0x07),
        command(baseAddress + 0x07) {}

IdeController::CommandRegisters::CommandRegisters() :
        data(0), error(0), features(0), sectorCount(0),
        sectorNumber(0), lbaLow(0), cylinderLow(0), lbaMid(0),
        cylinderHigh(0), lbaHigh(0), driveHead(0), status(0),
        command(0) {}

IdeController::ControlRegisters::ControlRegisters(uint16_t baseAddress) :
        alternateStatus(baseAddress + 0x02), deviceControl(baseAddress + 0x02), deviceAddress(baseAddress + 0x03) {}

IdeController::ControlRegisters::ControlRegisters() :
        alternateStatus(0), deviceControl(0), deviceAddress(0) {}

IdeController::DmaRegisters::DmaRegisters(uint16_t baseAddress) :
        command(baseAddress + 0x00), reserved0(baseAddress + 0x01), status(baseAddress + 0x02),
        reserved1(baseAddress + 0x03), address(baseAddress + 0x04) {}

IdeController::DmaRegisters::DmaRegisters() :
        command(0), reserved0(0), status(0),
        reserved1(0), address(0) {}

IdeController::ChannelRegisters::ChannelRegisters(uint16_t commandBaseAddress, uint16_t controlBaseAddress, uint16_t dmaBaseAddress) :
        command(commandBaseAddress), control(controlBaseAddress), dma(dmaBaseAddress) {}

IdeController::ChannelRegisters::ChannelRegisters()
        : command(0), control(0), dma(0) {}

void IdeController::initializeDrives() {
    auto devices = Util::ArrayList<IdeDevice*>();
    for (uint32_t i = 0; i < CHANNELS_PER_CONTROLLER; i++) {
        for (uint32_t j = 0; j < DEVICES_PER_CHANNEL; j++) {
            if (resetDrive(i, j)) {
                auto *device = identifyDrive(i, j);
                if (device != nullptr) {
                    devices.add(device);
                }
            }
        }
    }

    auto &storageService = Kernel::Service::getService<Kernel::StorageService>();
    for (auto *device : devices) {
        if (device->getDeviceInfo().type == ATA) {
            storageService.registerDevice(device, "ata");
        } else if (device->getDeviceInfo().type == ATAPI) {
            storageService.registerDevice(device, "atapi");
        }
    }
}

bool IdeController::resetDrive(uint8_t channel, uint8_t drive) {
    auto &registers = channels[channel];

    // Select drive
    if (!selectDrive(channel, drive)) {
        registers.driveType[drive] = OTHER;
        return false;
    }

    // Check drive presence
    auto status = registers.control.alternateStatus.readByte();
    if (status == 0) {
        registers.driveType[drive] = OTHER;
        return false;
    }

    // Set software reset bit on device control register
    registers.control.deviceControl.writeByte(0x04);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(5));

    // Clear software reset bit and disable interrupts
    registers.control.deviceControl.writeByte(0x02);
    registers.interruptsDisabled = true;
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(5));

    if (!waitBusy(registers.command.status)) {
        LOG_ERROR("Failed to reset drive [%u] on channel [%u]", drive, channel);
        registers.driveType[drive] = OTHER;
        return false;
    }

    // Check error register
    status = registers.command.error.readByte();
    if (status != 0x00 && status != 0x01) {
        LOG_ERROR("Got error code [%u] for drive [%u] on channel [%u]", status, drive, channel);
        registers.driveType[drive] = OTHER;
        return false;
    }

    auto sectorCount = registers.command.sectorCount.readByte();
    auto sectorNumber = registers.command.sectorNumber.readByte();
    if (sectorCount == 1 && sectorNumber == 1) {
        auto cylinderLow = registers.command.cylinderLow.readByte();
        auto cylinderHigh = registers.command.cylinderHigh.readByte();

        if ((cylinderLow == ATAPI_CYLINDER_LOW_V1 && cylinderHigh == ATAPI_CYLINDER_HIGH_V1) ||
            (cylinderLow == ATAPI_CYLINDER_LOW_V2 && cylinderHigh == ATAPI_CYLINDER_HIGH_V2)) {
            registers.driveType[drive] = ATAPI;
            return true;
        }

        if (cylinderLow == 0x00 && cylinderHigh == 0x00) {
            registers.driveType[drive] = ATA;
            return true;
        }

        return false;
    } else {
        LOG_ERROR("Sector count [%u] and sector number [%u] are not as expected for drive [%u] on channel [%u]", sectorCount, sectorNumber, drive, channel);
        registers.driveType[drive] = OTHER;
        return false;
    }
}

IdeDevice* IdeController::identifyDrive(uint8_t channel, uint8_t drive) {
    auto &registers = channels[channel];
    registers.control.deviceControl.writeByte(0x02);
    registers.interruptsDisabled = true;

    const auto type = registers.driveType[drive];
    if (type != ATA && type != ATAPI) {
        // Drive does not exist or has a type we cannot handle
        return nullptr;
    }

    if (!selectDrive(channel, drive)) {
        return nullptr;
    }

    DeviceInfo info{};
    auto *buffer = new uint16_t[256];

    if (registers.driveType[drive] == ATA) {
        if (!readAtaIdentity(channel, buffer)) {
            LOG_ERROR("Error while identifying ATA drive [%u] on channel [%u]", drive, channel);
            delete[] buffer;
            return nullptr;
        }

        info.type = ATA;
        info.signature = *(buffer + DEVICE_TYPE);
    } else if (registers.driveType[drive] == ATAPI) {
        if (!readAtapiIdentity(channel, buffer)) {
            LOG_ERROR("Error while identifying ATAPI drive [%u] on channel [%u]", drive, channel);
            delete[] buffer;
            return nullptr;
        }

        info.type = ATAPI;
        info.signature = *(buffer + DEVICE_TYPE);
        info.atapi.type = getAtapiType(info.signature);
        info.atapi.packetLength = info.signature % 2 ? 0x10 : 0x0c;

        auto *response = new uint8_t[8];
        if (readAtapiCapacity(channel, info.atapi.packetLength, reinterpret_cast<uint16_t*>(response))) {
            // A CD-ROM is present and the drive returned capacity information
            info.atapi.maxSectorsLba = (response[0] << 24) | (response[1] << 16) | (response[2] << 8) | response[3];
            info.atapi.blockSize = (response[4] << 24) | (response[5] << 16) | (response[6] << 8) | response[7];
            info.atapi.capacity = (info.atapi.maxSectorsLba + 1) * info.atapi.blockSize;
            delete[] response;
        }
    }

    info.channel = channel;
    info.drive = drive;
    info.cylinders = *(buffer + CYLINDERS);
    info.heads = *(buffer + HEADS);
    info.sectorsPerTrack = *(buffer + SECTORS);
    info.capabilities = *(buffer + CAPABILITIES);
    info.majorVersion = *(buffer + MAJOR_VERSION);
    info.minorVersion = *(buffer + MINOR_VERSION);
    info.maxSectorsLba28 = *(reinterpret_cast<uint32_t*>(buffer + MAX_LBA));
    info.maxSectorsLba48 = *(reinterpret_cast<uint32_t*>(buffer + MAX_LBA_EXT));
    info.multiwordDma = (*(buffer + DMA_MULTI)) >> 8;
    info.ultraDma = (*(buffer + UDMA_MODES)) >> 8;
    for (uint32_t j = 0; j < COMMAND_SET_WORD_COUNT; j++) {
        info.commandSets[j] = *(buffer + COMMAND_SETS + j);
    }

    if (info.commandSets[1] & 0x400) {
        // LBA48 supported
        info.addressing = LBA48;
    } else if (info.capabilities & 0x200) {
        // LBA28 supported
        info.addressing = LBA28;
    } else {
        info.addressing = CHS;
    }

    info.sectorSize = determineSectorSize(info);

    copyByteSwappedString(reinterpret_cast<const char*>(buffer + MODEL), info.model, sizeof(info.model));
    copyByteSwappedString(reinterpret_cast<const char*>(buffer + SERIAL), info.serial, sizeof(info.serial));
    copyByteSwappedString(reinterpret_cast<const char*>(buffer + FIRMWARE), info.firmware, sizeof(info.firmware));

    delete[] buffer;

    auto model = Util::String(reinterpret_cast<const uint8_t*>(info.model), sizeof(info.model)).strip();
    auto serial = Util::String(reinterpret_cast<const uint8_t*>(info.serial), sizeof(info.serial)).strip();
    auto firmware = Util::String(reinterpret_cast<const uint8_t*>(info.firmware), sizeof(info.firmware)).strip();
    LOG_INFO("Found %s drive on channel [%u]: %s %s (Firmware: [%s])", info.type == ATA ? "ATA" : "ATAPI", info.channel,
             static_cast<const char*>(model), static_cast<const char*>(serial), static_cast<const char*>(firmware));

    if (info.type == ATAPI && info.atapi.capacity == 0) {
        LOG_WARN("Failed to read capacity from drive [%u] on channel [%u] -> Probably no CD-ROM is present", drive, channel);
        return nullptr;
    } else {
        return new IdeDevice(*this, info);
    }
}

bool IdeController::readAtaIdentity(uint8_t channel, uint16_t *buffer) {
    auto &registers = channels[channel];
    registers.command.command.writeByte(IDENTIFY_ATA_DRIVE);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));

    if (!waitStatus(registers.control.alternateStatus, DATA_REQUEST)) {
        return false;
    }

    for (uint32_t i = 0; i < 256; i++) {
        buffer[i] = registers.command.data.readWord();
    }

    return true;
}

bool IdeController::readAtapiIdentity(uint8_t channel, uint16_t *buffer) {
    auto &registers = channels[channel];
    registers.command.command.writeByte(IDENTIFY_ATAPI_DRIVE);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));

    if (!waitStatus(registers.control.alternateStatus, DATA_REQUEST)) {
        return false;
    }

    for (uint32_t i = 0; i < 256; i++) {
        buffer[i] = registers.command.data.readWord();
    }

    return true;
}

bool IdeController::readAtapiCapacity(uint8_t channel, uint8_t packetLength, uint16_t *buffer) {
    auto &registers = channels[channel];

    auto *packet = new uint8_t[packetLength]{};
    packet[0] = READ_CAPACITY;

    if (!waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
        return false;
    }

    ioLock.acquire();
    prepareAtapiIO(channel, 8);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));

    if (!waitStatus(registers.control.alternateStatus, DATA_REQUEST)) {
        ioLock.release();
        delete[] packet;
        return false;
    }

    for (uint8_t i = 0; i < packetLength; i += 2) {
        registers.command.data.writeWord(packet[i] | (packet[i + 1] << 8));
    }

    if (!waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
        ioLock.release();
        delete[] packet;
        return false;
    }

    uint16_t answerSize = (registers.command.lbaHigh.readByte() << 8) | registers.command.lbaMid.readByte();
    for (uint8_t i = 0; i < answerSize / 2; i++) {
        *buffer++ = registers.command.data.readWord();
    }

    ioLock.release();
    delete[] packet;
    return true;
}

bool IdeController::selectDrive(uint8_t channel, uint8_t drive, bool prepareLbaAccess, uint8_t lbaHead) {
    auto &registers = channels[channel];
    if (!prepareLbaAccess && registers.lastDeviceControl != UINT8_MAX && ((registers.lastDeviceControl >> 4) & 0x01) == drive) {
        return true;
    }

    uint8_t selector = 0xa0 | (prepareLbaAccess << 6) | (drive << 4) | (0x0f & lbaHead);
    if (selector == registers.lastDeviceControl) {
        return true;
    }

    if (!waitBusy(registers.command.status)) {
        return false;
    }

    registers.command.driveHead.writeByte(selector);
    Kernel::Service::getService<Kernel::TimeService>().busyWait(Util::Time::Timestamp::ofNanoseconds(400));

    if (!waitBusy(registers.command.status)) {
        return false;
    }

    channels[channel].lastDeviceControl = selector;
    return true;
}

void IdeController::initializeAvailableControllers() {
    auto devices = Pci::search(Pci::Class::MASS_STORAGE, PCI_SUBCLASS_IDE);
    for (const auto &device : devices) {
        auto *controller = new IdeController(device);
        controller->plugin();
        controller->initializeDrives();
    }
}

void IdeController::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::PRIMARY_ATA, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::PRIMARY_ATA);
    interruptService.assignInterrupt(Kernel::InterruptVector::SECONDARY_ATA, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::SECONDARY_ATA);
}

void IdeController::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    if (slot == Kernel::InterruptVector::PRIMARY_ATA) {
        channels[0].receivedInterrupt = true;
    } else if (slot == Kernel::InterruptVector::SECONDARY_ATA) {
        channels[1].receivedInterrupt = true;
    }
}

uint8_t IdeController::getAtapiType(uint16_t signature) {
    uint8_t atapiIdentifier = (signature & 0x0f00) >> 8;
    switch(atapiIdentifier){
        case 0x00:
            return SBC_3;
        case 0x04:
            return SBC;
        case 0x05:
            return MMC;
        case 0x07:
            return SBC;
        case 0x0C:
            return SCC_2;
        case 0x0E:
            return RBC;
        default:
            return UNDEFINED;
    }

}

uint16_t IdeController::determineSectorSize(const DeviceInfo &info) {
    if (info.type == ATAPI) {
        return ATAPI_SECTOR_SIZE;
    }

    auto &registers = channels[info.channel];
    uint32_t sectorSize = 0;

    prepareAtaIO(info, 0, 1);
    registers.command.command.writeByte(READ_PIO_LBA28);

    uint16_t timeout = WAIT_ON_STATUS_TIMEOUT;
    while (waitStatus(registers.control.alternateStatus, DATA_REQUEST, timeout)) {
        for (uint32_t i = 0; i < 128; i++) {
            registers.command.data.readWord();
        }

        sectorSize += 256;
        timeout = 0xff;
    }

    return sectorSize;
}

uint16_t IdeController::performAtaIO(const DeviceInfo &info, TransferMode mode, uint8_t *buffer, uint64_t startSector, uint32_t sectorCount) {
    auto &registers = channels[info.channel];
    if (!checkBounds(info, startSector, sectorCount)) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "IDE: Trying to read/write out of disk bounds!");
    }

    ioLock.acquire();
    if (!selectDrive(info.channel, info.drive)) {
        ioLock.release();
        return 0;
    }

    // Enable interrupts
    if (registers.interruptsDisabled) {
        registers.control.deviceControl.writeByte(0x00);
        registers.interruptsDisabled = false;
    }

    // Clear interrupt flag
    registers.receivedInterrupt = false;

    if (!waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
        ioLock.release();
        return 0;
    }

    uint16_t maxSectorCount = info.addressing == LBA48 ? 0xffff : 0xff;
    uint32_t processedSectors = 0;
    while (processedSectors < sectorCount) {
        uint32_t sectorsLeft = sectorCount - processedSectors;
        uint32_t start = startSector + processedSectors;
        uint32_t count = sectorsLeft > maxSectorCount ? maxSectorCount : sectorsLeft;

        // DMA transfers sometimes have issues on real hardware, and are actually slower than programmed I/O, because of the overhead of copying data to/from the DMA buffer.
        // Therefore, we use programmed I/O for now, until we hava a proper I/O management that can handle DMA transfers more efficiently.
        uint16_t sectors = performDmaAtaIO(info, mode, reinterpret_cast<uint16_t*>(buffer + (processedSectors * info.sectorSize)), start, count);

        /*uint16_t sectors;
        if (supportsDma && info.supportsDma()) {
            sectors = performDmaAtaIO(info, mode, reinterpret_cast<uint16_t*>(buffer + (processedSectors * info.sectorSize)), start, count);
        } else {
            sectors = performProgrammedAtaIO(info, mode, reinterpret_cast<uint16_t*>(buffer + (processedSectors * info.sectorSize)), start, count);
        }*/

        processedSectors += sectors;
        if (sectors == 0) {
            ioLock.release();
            return processedSectors;
        }
    }

    ioLock.release();
    return processedSectors;
}

void IdeController::prepareAtaIO(const DeviceInfo &info, uint64_t startSector, uint16_t sectorCount) {
    auto &registers = channels[info.channel];

    if (info.addressing == CHS) {
        auto chsConverter = ChsConverter(info.cylinders, info.heads, info.sectorsPerTrack);
        auto chs = chsConverter.lbaToChs(startSector);

        selectDrive(info.channel, info.drive, false, chs.head);
        registers.command.sectorCount.writeByte(sectorCount);
        registers.command.sectorNumber.writeByte(chs.sector);
        registers.command.cylinderLow.writeByte(chs.cylinder);
        registers.command.cylinderHigh.writeByte(chs.cylinder >> 8);
    } else if (info.addressing == LBA28) {
        selectDrive(info.channel, info.drive, true, startSector >> 24);
        registers.command.sectorCount.writeByte(sectorCount);
        registers.command.lbaLow.writeByte(startSector);
        registers.command.lbaMid.writeByte(startSector >> 8);
        registers.command.lbaHigh.writeByte(startSector >> 16);
    } else if (info.addressing == LBA48) {
        selectDrive(info.channel, info.drive, true, 0);
        // First wave
        registers.command.sectorCount.writeByte(sectorCount >> 8);
        registers.command.lbaLow.writeByte(startSector >> 24);
        registers.command.lbaMid.writeByte(startSector >> 32);
        registers.command.lbaHigh.writeByte(startSector >> 40);
        // Second wave
        registers.command.sectorCount.writeByte(sectorCount);
        registers.command.lbaLow.writeByte(startSector);
        registers.command.lbaMid.writeByte(startSector >> 8);
        registers.command.lbaHigh.writeByte(startSector >> 16);
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IDE: Unsupported address type!");
    }
}

uint16_t IdeController::performProgrammedAtaIO(const DeviceInfo &info, TransferMode mode, uint16_t *buffer, uint64_t startSector, uint16_t sectorCount) {
    auto &registers = channels[info.channel];
    prepareAtaIO(info, startSector, sectorCount);

    uint8_t command;
    if (info.addressing == CHS || info.addressing == LBA28) {
        command = mode == WRITE ? WRITE_PIO_LBA28 : READ_PIO_LBA28;
    } else if (info.addressing == LBA48) {
        command = mode == WRITE ? WRITE_PIO_LBA48 : READ_PIO_LBA48;
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IDE: Unsupported address type!");
    }

    registers.command.command.writeByte(command);

    if (!waitStatus(registers.control.alternateStatus, DATA_REQUEST)) {
        return 0;
    }

    uint32_t i;
    for (i = 0; i < sectorCount; i++) {
        if (i > 0 && !waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
            return i;
        }

        if (mode == READ) {
            for (uint32_t j = 0; j < info.sectorSize / 2; j++) {
                *buffer++ = registers.command.data.readWord();
            }
        } else if (mode == WRITE) {
            for (uint32_t j = 0; j < info.sectorSize / 2; j++) {
                registers.command.data.writeWord(*buffer++);
            }
        } else {
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IDE: Unsupported transfer mode!");
        }
    }

    return i;
}

uint16_t IdeController::performDmaAtaIO(const DeviceInfo &info, TransferMode mode, uint16_t *buffer, uint64_t startSector, uint16_t sectorCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto &registers = channels[info.channel];

    uint8_t command;
    if (info.addressing == LBA28) {
        command = mode == WRITE ? WRITE_DMA_LBA28 : READ_DMA_LBA28;
    } else if (info.addressing == LBA48) {
        command = mode == WRITE ? WRITE_DMA_LBA48 : READ_DMA_LBA48;
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IDE: Unsupported address type!");
    }

    // Calculate the amount of pages needed for the operation
    auto size = sectorCount * info.sectorSize;
    auto pages = size / Util::PAGESIZE + (size % Util::PAGESIZE == 0 ? 0 : 1);

    // Each page corresponds to an 8-byte entry in the PRD
    auto prdSize = pages * 8;
    auto prdPages = prdSize / Util::PAGESIZE + (prdSize % Util::PAGESIZE == 0 ? 0 : 1);
    auto prdVirtual = reinterpret_cast<uint32_t*>(memoryService.mapIO(prdPages));
    auto prdPhysical = reinterpret_cast<uint32_t>(memoryService.getPhysicalAddress(prdVirtual));

    // Allocate memory for the DMA transfer
    auto dmaMemoryVirtual = reinterpret_cast<uint32_t*>(memoryService.mapIO(pages));
    auto dmaMemoryPhysical = reinterpret_cast<uint32_t>(memoryService.getPhysicalAddress(dmaMemoryVirtual));

    if (mode == WRITE) {
        auto source = Util::Address<uint32_t>(buffer);
        auto target = Util::Address<uint32_t>(dmaMemoryVirtual);
        target.copyRange(source, size);
    }

    // Fill PRD
    uint32_t i;
    for (i = 0; i < (pages - 1); i++) {
        prdVirtual[2 * i] = dmaMemoryPhysical + (Util::PAGESIZE * i);
        prdVirtual[(2 * i) + 1] = Util::PAGESIZE;
    }

    // Set last PRD entry wit EOT bit
    prdVirtual[2 * i] = dmaMemoryPhysical + (Util::PAGESIZE * i);
    prdVirtual[(2 * i) + 1] = (size % Util::PAGESIZE) | PRD_END_OF_TRANSMISSION;

    // Prepare DMA transfer to physical address
    registers.dma.address.writeDoubleWord(prdPhysical);

    // Set DMA direction
    registers.dma.command.writeByte(mode == READ ? 0x00 : DmaCommand::DIRECTION);

    // Clear interrupt and error bits
    registers.dma.status.writeByte(~(DmaStatus::DMA_ERROR | DmaStatus::INTERRUPT));

    // Select drive and sector
    prepareAtaIO(info, startSector, sectorCount);

    // Send command
    registers.command.command.writeByte(command);
    if (!waitStatus(registers.control.alternateStatus, DATA_REQUEST)) {
        delete prdVirtual;
        delete dmaMemoryVirtual;
        return 0;
    }

    // Start DMA transfer
    registers.receivedInterrupt = false;
    registers.dma.command.writeByte(DmaCommand::ENABLE);

    uint32_t timeout = Util::Time::getSystemTime().toMilliseconds() + DMA_TIMEOUT;
    do {
        if (registers.receivedInterrupt) {
            // Stop DMA transfer and check flags
            registers.dma.command.writeByte(0x00);

            auto dmaStatus = registers.dma.status.readByte();
            if ((dmaStatus & DmaStatus::INTERRUPT) == DmaStatus::INTERRUPT) {
                // An interrupt has been fired -> Check if bus master is still enabled
                if ((dmaStatus & DmaStatus::BUS_MASTER_ACTIVE) == DmaStatus::BUS_MASTER_ACTIVE) {
                    // Continue DMA transfer
                    registers.receivedInterrupt = false;
                    registers.dma.command.writeByte(DmaCommand::ENABLE);
                } else {
                    // DMA transfer is finished
                    break;
                }
            }
        }
    } while (Util::Time::getSystemTime().toMilliseconds() < timeout);

    if (Util::Time::getSystemTime().toMilliseconds() >= timeout) {
        delete prdVirtual;
        delete dmaMemoryVirtual;
        return 0;
    }

    if (mode == READ) {
        auto source = Util::Address<uint32_t>(dmaMemoryVirtual);
        auto target = Util::Address<uint32_t>(buffer);
        target.copyRange(source, size);
    }

    delete prdVirtual;
    delete dmaMemoryVirtual;
    return sectorCount;
}

void IdeController::prepareAtapiIO(uint8_t channel, uint16_t len) {
    auto &registers = channels[channel];

    if (len == 0) {
        len = 8;
    }

    registers.command.features.writeByte(0);
    registers.command.sectorCount.writeByte(0);
    registers.command.lbaLow.writeByte(0);
    registers.command.lbaMid.writeByte((len >> 0) & 0xff);
    registers.command.lbaHigh.writeByte((len >> 8) & 0xff);
    registers.command.command.writeByte(ATA_SEND_PACKET);
}

uint16_t IdeController::performAtapiIO(const IdeController::DeviceInfo &info, IdeController::TransferMode mode, uint8_t *buffer, uint64_t startSector, uint32_t sectorCount) {
    auto &registers = channels[info.channel];

    if (mode != READ) {
        return 0;
    }

    if (startSector + sectorCount >= info.atapi.maxSectorsLba) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "IDE: Trying to read/write out of disk bounds!");
    }

    ioLock.acquire();
    if (!selectDrive(info.channel, info.drive)) {
        ioLock.release();
        return 0;
    }

    // Enable interrupts
    if (registers.interruptsDisabled) {
        registers.control.deviceControl.writeByte(0x00);
        registers.interruptsDisabled = false;
    }

    // Clear interrupt flag
    registers.receivedInterrupt = false;

    if (!waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
        ioLock.release();
        return 0;
    }

    uint32_t processedSectors = 0;
    while (processedSectors < sectorCount) {
        uint32_t sectorsLeft = sectorCount - processedSectors;
        uint32_t start = startSector + processedSectors;
        uint32_t count = sectorsLeft > 65535 / info.atapi.blockSize ? 65535 / info.atapi.blockSize : sectorsLeft;

        uint16_t sectors;
        sectors = performProgrammedAtapiIO(info, mode, reinterpret_cast<uint16_t*>(buffer + (processedSectors * info.sectorSize)), start, count);

        processedSectors += sectors;
        if (sectors == 0) {
            ioLock.release();
            return processedSectors;
        }
    }

    ioLock.release();
    return processedSectors;
}

uint16_t IdeController::performProgrammedAtapiIO(const IdeController::DeviceInfo &info, IdeController::TransferMode mode, uint16_t *buffer, uint64_t startSector, uint16_t sectorCount) {
    auto &registers = channels[info.channel];

    if (mode != READ) {
        return 0;
    }

    prepareAtapiIO(info.channel, static_cast<uint16_t>(sectorCount * info.sectorSize));

    // packet for read command
    auto *packet = new uint8_t[info.atapi.packetLength]{};
    packet[0] = ATAPI_READ;
    packet[2] = (startSector >> 24) & 0xff;
    packet[3] = (startSector >> 16) & 0xff;
    packet[4] = (startSector >> 8) & 0xff;
    packet[5] = (startSector >> 0) & 0xff;
    packet[6] = (sectorCount >> 24) & 0xff;
    packet[7] = (sectorCount >> 16) & 0xff;
    packet[8] = (sectorCount >> 8) & 0xff;
    packet[9] = (sectorCount >> 0) & 0xff;

    if (!waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
        ioLock.release();
        delete[] packet;
        return 0;
    }

    for (uint8_t i = 0; i < info.atapi.packetLength; i += 2) {
        registers.command.data.writeWord(packet[i] | (packet[i + 1] << 8));
    }

    if (!waitStatus(registers.control.alternateStatus, DATA_REQUEST)) {
        delete[] packet;
        return 0;
    }

    uint16_t transferredBytes = 0;
    while (transferredBytes < sectorCount * info.sectorSize) {
        if (transferredBytes > 0 && !waitStatus(registers.control.alternateStatus, DRIVE_READY)) {
            delete[] packet;
            return transferredBytes / info.sectorSize;
        }

        uint16_t transferSize = (registers.command.lbaHigh.readByte() << 8) | registers.command.lbaMid.readByte();
        for (uint16_t j = 0; j < transferSize / 2; j++) {
            *buffer++ = registers.command.data.readWord();
        }

        transferredBytes += transferSize;
    }

    delete[] packet;
    return transferredBytes / info.sectorSize;
}

bool IdeController::waitStatus(const IoPort &port, Status status, uint16_t timeout) {
    uint32_t endTime = Util::Time::getSystemTime().toMilliseconds() + timeout;

    do {
        auto currentStatus = port.readByte();
        if ((currentStatus & BUSY) == BUSY) {
            continue;
        }

        if ((currentStatus & ERROR) == ERROR) {
            LOG_ERROR("Error while waiting on status [0x%02x]", status);
            return false;
        }

        if ((currentStatus & status) == status) {
            return true;
        }
    } while (timeout == 0 || Util::Time::getSystemTime().toMilliseconds() < endTime);

    // Timeout occurred
    // Do not log an error, as this may be normal behavior (e.g. in 'determine_ata_sector_size()')
    return false;
}

bool IdeController::waitBusy(const IoPort &port, uint16_t timeout) {
    return waitStatus(port, NONE, timeout);
}

void IdeController::copyByteSwappedString(const char *source, char *target, uint32_t length) {
    for (uint32_t i = 0; i < length; i += 2) {
        target[i] = source[i + 1];
        target[i + 1] = source[i];
    }
}

bool IdeController::checkBounds(const DeviceInfo &info, uint64_t startSector, uint32_t sectorCount) {
    switch (info.addressing) {
        case CHS: {
            auto converter = ChsConverter(info.cylinders, info.heads, info.sectorsPerTrack);
            auto chs = converter.lbaToChs(startSector + sectorCount);
            return chs.cylinder < info.cylinders;
        }
        case LBA28:
            return startSector + sectorCount < info.maxSectorsLba28;
        case LBA48:
            return startSector + sectorCount < info.maxSectorsLba48;
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "IDE: Unsupported address type!");
    }
}

bool IdeController::DeviceInfo::supportsDma() const {
    return ultraDma != 0 || multiwordDma != 0;
}

}