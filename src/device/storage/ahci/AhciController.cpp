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
 * The AHCI driver is based on a bachelor's thesis, written by Manuel Demetrio Angelescu.
 * The original source code can be found here: https://github.com/m8nu/hhuOS
 */

#include "AhciController.h"

#include "kernel/log/Log.h"
#include "kernel/service/Service.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/service/InterruptService.h"
#include "lib/util/base/Constants.h"
#include "AhciDevice.h"
#include "kernel/service/StorageService.h"
#include "device/bus/pci/Pci.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Storage {

AhciController::AhciController(const PciDevice &pciDevice) : pciDevice(pciDevice) {
    LOG_INFO("Initializing controller [0x%04x:0x%04x]", pciDevice.getVendorId(), pciDevice.getDeviceId());
    uint16_t command = pciDevice.readWord(Pci::COMMAND);
    command |= Pci::MEMORY_SPACE | Pci::BUS_MASTER;
    pciDevice.writeWord(Pci::COMMAND, command);

    // Read base address and map MMIO registers
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto *abar = reinterpret_cast<void*>(pciDevice.readDoubleWord(Pci::BASE_ADDRESS_5) & 0xfffffffc);
    registers = static_cast<HbaRegisters*>(memoryService.mapIO(abar, 1));
    LOG_INFO("AHCI [%u.%u.%u] compatible controller found", ((registers->version & 0xffff0000) >> 16), (registers->version & 0x0000ff00 >> 8), registers->version & 0x000000ff);

    biosHandoff();
    enableAhci();

    registers->interruptStatus = 0xffffffff; // Clear global interrupt status

    // Read maximum number of supported ports from lowest 5 bits of capabilities registers
    portCount = (registers->hostCapabilities & 0x0000001f) + 1;
    LOG_INFO("[%u] ports supported", portCount);

    // Allocate port structures
    virtualCommandLists = new HbaCommandHeader*[portCount]{};
    portLocks = new Util::Async::Spinlock[portCount]{};

    LOG_INFO("Scanning ports for devices");
    for (uint32_t i = 0; i < portCount; i++) {
        if (registers->portsImplemented & (1 << i)) {
            auto &port = registers->ports[i];
            auto type = port.signature;

            if (type == ATA || type == ATAPI) {
                rebasePort(i);
                port.sataError = 0xffffffff; // Clear errors
                port.interruptStatus = 0xffffffff; // Clear port interrupt status
                port.interruptEnable = 0x00000000; // Disable all interrupts

                auto *info = identifyDevice(i);
                if (info == nullptr) {
                    LOG_ERROR("Failed to identify %s pciDevice on port [%u]", type == ATA ? "ATA" : "ATAPI", i);
                    port.stopCommandEngine();
                    continue;
                }

                auto model = Util::String(reinterpret_cast<const uint8_t*>(info->model), sizeof(DeviceInfo::model)).strip();
                auto serial = Util::String(reinterpret_cast<const uint8_t*>(info->serialNumber), sizeof(DeviceInfo::serialNumber)).strip();
                auto firmware = Util::String(reinterpret_cast<const uint8_t*>(info->firmwareRevision), sizeof(DeviceInfo::firmwareRevision)).strip();
                LOG_INFO("Found %s drive on port [%u]: %s %s (Firmware: [%s])", type == ATA ? "ATA" : "ATAPI", i, static_cast<const char*>(model), static_cast<const char*>(serial), static_cast<const char*>(firmware));

                auto *device = new AhciDevice(i, info, *this);
                Kernel::Service::getService<Kernel::StorageService>().registerDevice(device, type == ATA ? "ata" : "atapi");
            } else {
                switch (type) {
                    case ENCLOSURE_POWER_MANAGEMENT_BRIDGE:
                        LOG_INFO("Found enclosure power management bridge on port [%u]", i);
                        break;
                    case PORT_MULTIPLIER:
                        LOG_INFO("Found port multiplier on port [%u]", i);
                        break;
                    default:
                        break;
                }

                port.stopCommandEngine();
            }
        }
    }
}

AhciController::~AhciController() {
    for (uint32_t i = 0; i < portCount; i++) {
        delete virtualCommandLists[i];
    }

    delete portLocks;
    delete virtualCommandLists;
    delete registers;
}

void AhciController::initializeAvailableControllers() {
    Util::Array<PciDevice> devices = Pci::search(Pci::Class::MASS_STORAGE, PCI_SUBCLASS_AHCI);
    for (const auto &device: devices) {
        auto *controller = new AhciController(device);
        controller->plugin();
    }
}

bool AhciController::biosHandoff() {
    // BIOS handoff has been introduced with AHCI version 1.2
    if (registers->version < 0x10200) {
        LOG_INFO("Skipping BIOS handoff, because of implemented AHCI version");
        return true;
    }

    // Check if BIOS handoff is supported by the controller
    if (!(registers->extendedHostCapabilities & 0x01)) {
        LOG_INFO("BIOS handoff is not supported");
        return true;
    }

    // Take ownership of the controller
    LOG_INFO("Performing BIOS handoff");
    registers->biosHandoffControl |= OS_OWNED_SEMAPHORE;
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(25));

    // Wait for BIOS
    while (registers->biosHandoffControl & BIOS_BUSY) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(100));
    }

    // Check if we have the ownership now
    if ((registers->biosHandoffControl & (BIOS_OWNED_SEMAPHORE | OS_OWNED_SEMAPHORE | BIOS_BUSY)) != OS_OWNED_SEMAPHORE) {
        LOG_ERROR("BIOS handoff failed");
        return false;
    }

    // Clear the ownership change bit
    registers->biosHandoffControl &= ~OS_OWNERSHIP_CHANGE;

    return true;
}

bool AhciController::enableAhci() {
    uint32_t time = 0;

    LOG_INFO("Enabling AHCI");
    registers->globalHostControl |= AHCI_ENABLE;

    do {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        time += 10;
    } while (!(registers->globalHostControl & AHCI_ENABLE) && time < AHCI_ENABLE_TIMEOUT_MS);

    if (!(registers->globalHostControl & AHCI_ENABLE)) {
        LOG_ERROR("Failed to enable AHCI");
        return false;
    }

    return true;
}

void AhciController::rebasePort(uint32_t portNumber) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto &port = registers->ports[portNumber];

    LOG_INFO("Rebasing port [%u]", portNumber);

    // Make sure, no commands are being processed during rebase
    port.stopCommandEngine();

    // Allocate memory for command list
    virtualCommandLists[portNumber] = static_cast<HbaCommandHeader*>(memoryService.mapIO(1));
    port.commandListBaseAddress = reinterpret_cast<uint32_t>(memoryService.getPhysicalAddress(virtualCommandLists[portNumber]));
    Util::Address<uint32_t>(virtualCommandLists[portNumber]).setRange(0, Util::PAGESIZE);

    // Port may now process commands again
    port.startCommandEngine();
}

uint32_t AhciController::findCommandSlot(uint32_t portNumber) {
    auto &port = registers->ports[portNumber];

    uint32_t slotCount = ((registers->hostCapabilities >> 8) & 0x0000001f) + 1;
    uint32_t slotStatus = (port.sataActive | port.commandIssue); // Set bits correspond to currently occupied slots
    for (uint32_t i = 0; i < slotCount; i++) {
        if (!(slotStatus & (1 << i))) {
            return i;
        }
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "AHCI: No command slot available!");
}

void AhciController::byteSwapString(char *string, uint32_t length) {
    for (uint32_t i = 0; i < length; i += 2) {
        const auto tmp = string[i];
        string[i] = string[i + 1];
        string[i + 1] = tmp;
    }
}

AhciController::DeviceInfo* AhciController::identifyDevice(uint32_t portNumber) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto &port = registers->ports[portNumber];
    auto *commandList = virtualCommandLists[portNumber];

    portLocks[portNumber].acquire();

    if (!port.isActive()) {
        LOG_ERROR("Trying to identify device on an inactive port");
        portLocks[portNumber].release();
        return nullptr;
    }

    auto slot = findCommandSlot(portNumber);

    auto *dataBaseAddress = memoryService.mapIO(1);
    auto *dataBaseAddressPhysical = memoryService.getPhysicalAddress(dataBaseAddress);

    auto *commandTable = HbaCommandTable::createCommandTable(1);
    auto &commandFis = *reinterpret_cast<FisRegisterHostToDevice*>(commandTable->commandFis);
    commandFis.type = REGISTER_HOST_TO_DEVICE;
    commandFis.commandControl = 1;
    commandFis.command = ATA_IDENTIFY;

    auto &descriptorEntry = commandTable->physicalRegionDescriptorTable[0];
    descriptorEntry.dataBaseAddress = reinterpret_cast<uint32_t>(dataBaseAddressPhysical);
    descriptorEntry.dataByteCount = 512 - 1;

    auto &commandHeader = commandList[slot];
    commandHeader.clear();
    commandHeader.physicalRegionDescriptorTableLength = 1;
    commandHeader.commandFisLength = sizeof(FisRegisterHostToDevice) / sizeof(uint32_t);
    commandHeader.commandTableDescriptorBaseAddress = reinterpret_cast<uint32_t>(memoryService.getPhysicalAddress(commandTable));

    // Wait while device is busy
    uint32_t timeout = Util::Time::getSystemTime().toMilliseconds() + COMMAND_TIMEOUT;
    while (port.taskFileData & (BUSY | DATA_TRANSFER_REQUESTED)) {
        if (Util::Time::getSystemTime().toMilliseconds() >= timeout) {
            LOG_ERROR("Timeout while waiting on busy device");
            portLocks[portNumber].release();
            delete reinterpret_cast<uint8_t*>(dataBaseAddress);
            delete commandTable;
            return nullptr;
        }

        Util::Async::Thread::yield();
    }

    // Issue command
    port.commandIssue = 1 << slot;

    // Wait for command completion
    timeout = Util::Time::getSystemTime().toMilliseconds() + COMMAND_TIMEOUT;
    while (true) {
        if (!(port.commandIssue & (1 << slot))) {
            break;
        }

        if (port.interruptStatus & TASK_FILE_ERROR) {
            LOG_ERROR("Read error during ATA_IDENTIFY command");
            portLocks[portNumber].release();
            delete commandTable;
            delete reinterpret_cast<uint8_t*>(dataBaseAddress);
            return nullptr;
        }

        if (Util::Time::getSystemTime().toMilliseconds() >= timeout) {
            LOG_ERROR("Timeout during ATA_IDENTIFY command");
            portLocks[portNumber].release();
            delete reinterpret_cast<uint8_t*>(dataBaseAddress);
            delete commandTable;
            return nullptr;
        }

        Util::Async::Thread::yield();
    }

    auto *deviceInfo = static_cast<DeviceInfo*>(dataBaseAddress);
    byteSwapString(reinterpret_cast<char*>(deviceInfo->model), sizeof(DeviceInfo::model));
    byteSwapString(reinterpret_cast<char*>(deviceInfo->serialNumber), sizeof(DeviceInfo::serialNumber));
    byteSwapString(reinterpret_cast<char*>(deviceInfo->firmwareRevision), sizeof(DeviceInfo::firmwareRevision));

    portLocks[portNumber].release();
    delete commandTable;
    return deviceInfo;
}

uint16_t AhciController::performAtaIO(uint32_t portNumber, const DeviceInfo &deviceInfo, AhciController::TransferMode mode, uint8_t *buffer, uint64_t startSector, uint32_t sectorCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto &port = registers->ports[portNumber];
    auto *commandList = virtualCommandLists[portNumber];

    portLocks[portNumber].acquire();

    if (!port.isActive()) {
        portLocks[portNumber].release();
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Trying to read/write from a device on an inactive port");
    }

    auto slot = findCommandSlot(portNumber);

    const auto dmaSize = sectorCount * deviceInfo.bytesPerSector;
    const auto dmaPages = dmaSize % Util::PAGESIZE == 0 ? (dmaSize / Util::PAGESIZE) : (dmaSize / Util::PAGESIZE) + 1;
    auto *dmaBuffer = memoryService.allocateIsaMemory(dmaPages);
    auto *physicalDmaAddress = memoryService.getPhysicalAddress(dmaBuffer);

    if (mode == WRITE) {
        auto sourceAddress = Util::Address<uint32_t>(buffer);
        auto targetAddress = Util::Address<uint32_t>(dmaBuffer);
        targetAddress.copyRange(sourceAddress, sectorCount * deviceInfo.bytesPerSector);
    }

    auto descriptorCount = sectorCount % SECTORS_PER_DESCRIPTOR_ENTRY == 0 ? (sectorCount / SECTORS_PER_DESCRIPTOR_ENTRY) : (sectorCount / SECTORS_PER_DESCRIPTOR_ENTRY) + 1;

    auto *commandTable = HbaCommandTable::createCommandTable(descriptorCount);
    for (uint32_t i = 0; i < descriptorCount; i++) {
        auto &entry = commandTable->physicalRegionDescriptorTable[i];

        uint32_t remainingSectors = sectorCount - (i * SECTORS_PER_DESCRIPTOR_ENTRY);
        entry.dataBaseAddress = reinterpret_cast<uint32_t>(physicalDmaAddress) + i * SECTORS_PER_DESCRIPTOR_ENTRY * deviceInfo.bytesPerSector;
        entry.dataByteCount = ((remainingSectors < SECTORS_PER_DESCRIPTOR_ENTRY ? remainingSectors : SECTORS_PER_DESCRIPTOR_ENTRY) * deviceInfo.bytesPerSector) - 1;
    }

    auto &commandFis = *reinterpret_cast<FisRegisterHostToDevice*>(commandTable->commandFis);
    commandFis.type = REGISTER_HOST_TO_DEVICE;
    commandFis.commandControl = 1;
    commandFis.command = mode == READ ? READ_DMA_EX : WRITE_DMA_EX;
    commandFis.device = 1 << 6; // LBA mode
    commandFis.lba0 = startSector & 0xff;
    commandFis.lba1 = (startSector >> 8) & 0xff;
    commandFis.lba2 = (startSector >> 16) & 0xff;
    commandFis.lba3 = (startSector >> 24) & 0xff;
    commandFis.countLow = sectorCount & 0xff;
    commandFis.countHigh = (sectorCount >> 8) & 0xff;

    auto &commandHeader = commandList[slot];
    commandHeader.clear();
    commandHeader.physicalRegionDescriptorTableLength = descriptorCount; // Read 8 sectors per descriptor
    commandHeader.commandFisLength = sizeof(FisRegisterHostToDevice) / sizeof(uint32_t);
    commandHeader.commandTableDescriptorBaseAddress = reinterpret_cast<uint32_t>(memoryService.getPhysicalAddress(commandTable));
    commandHeader.write = mode == READ ? 0 : 1;

    // Wait while device is busy
    uint32_t timeout = Util::Time::getSystemTime().toMilliseconds() + COMMAND_TIMEOUT;
    while (port.taskFileData & (BUSY | DATA_TRANSFER_REQUESTED)) {
        if (Util::Time::getSystemTime().toMilliseconds() >= timeout) {
            portLocks[portNumber].release();
            delete reinterpret_cast<uint8_t*>(dmaBuffer);
            delete commandTable;
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Timeout while waiting on busy device");
        }

        Util::Async::Thread::yield();
    }

    // Issue command
    port.commandIssue = 1 << slot;

    // Wait for command completion
    timeout = Util::Time::getSystemTime().toMilliseconds() + COMMAND_TIMEOUT;
    while (true) {
        if (!(port.commandIssue & (1 << slot))) {
            break;
        }

        if (port.interruptStatus & TASK_FILE_ERROR) {
            portLocks[portNumber].release();
            delete reinterpret_cast<uint8_t*>(dmaBuffer);
            delete commandTable;
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Error during DMA command");
        }

        if (Util::Time::getSystemTime().toMilliseconds() >= timeout) {
            portLocks[portNumber].release();
            delete reinterpret_cast<uint8_t*>(dmaBuffer);
            delete commandTable;
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Timeout during DMA command");
        }

        Util::Async::Thread::yield();
    }

    if (mode == READ) {
        auto sourceAddress = Util::Address<uint32_t>(dmaBuffer);
        auto targetAddress = Util::Address<uint32_t>(buffer);
        targetAddress.copyRange(sourceAddress, deviceInfo.bytesPerSector * sectorCount);
    }

    portLocks[portNumber].release();
    delete reinterpret_cast<uint8_t*>(dmaBuffer);
    delete commandTable;
    return sectorCount;
}

void AhciController::trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) {}

void AhciController::plugin() {
    auto &interruptService = Kernel::InterruptService::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(static_cast<Kernel::InterruptVector>(pciDevice.getInterruptLine() + 32), *this);
    interruptService.allowHardwareInterrupt(pciDevice.getInterruptLine());
}

void AhciController::HbaPort::startCommandEngine() {
    // Wait until the controller has stopped processing commands
    while (command & COMMAND_LIST_RUNNING) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
    }

    command |= (START | FIS_RECEIVE_ENABLE);
}

void AhciController::HbaPort::stopCommandEngine() {
    // Clear start and FIS receive bits
    command &= ~(START | FIS_RECEIVE_ENABLE);

    // Wait until the controller has stopped processing commands
    while (command & (FIS_RECEIVE_RUNNING | COMMAND_LIST_RUNNING)) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
    }
}

bool AhciController::HbaPort::isActive() const {
    return ((command & (START | FIS_RECEIVE_ENABLE | FIS_RECEIVE_RUNNING | COMMAND_LIST_RUNNING)) == (START | FIS_RECEIVE_ENABLE | FIS_RECEIVE_RUNNING | COMMAND_LIST_RUNNING));
}

AhciController::DeviceSignature AhciController::HbaPort::checkType() const {
    auto detection = static_cast<DeviceDetection>(sataStatus & 0x0f);
    auto powerManagement = static_cast<InterfacePowerManagement>((sataStatus >> 8) & 0x0f);

    if (detection != PRESENT) {
        return NONE;
    }
    if (powerManagement != ACTIVE) {
        return NONE;
    }

    return static_cast<DeviceSignature>(signature);
}

void AhciController::HbaCommandHeader::clear() {
    Util::Address<uint32_t>(this).setRange(0, sizeof(uint32_t) * 2);
}

AhciController::HbaCommandTable* AhciController::HbaCommandTable::createCommandTable(uint16_t descriptorCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();

    auto tableSize = sizeof(commandFis) + sizeof(atapiCommand) + sizeof(reserved) + descriptorCount * sizeof(HbaPhysicalRegionDescriptorTableEntry);
    auto tablePages = tableSize % Util::PAGESIZE == 0 ? (tableSize / Util::PAGESIZE) : (tableSize / Util::PAGESIZE) + 1;
    auto *commandTable = reinterpret_cast<HbaCommandTable*>(memoryService.mapIO(tablePages));

    Util::Address<uint32_t>(commandTable).setRange(0, tableSize);
    return commandTable;
}

}