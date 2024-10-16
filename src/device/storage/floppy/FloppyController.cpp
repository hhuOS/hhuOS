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

#include "kernel/service//InterruptService.h"
#include "FloppyController.h"
#include "FloppyDevice.h"
#include "device/time/rtc/Cmos.h"
#include "kernel/service/MemoryService.h"
#include "device/cpu/Cpu.h"
#include "kernel/service/StorageService.h"
#include "lib/util/async/Thread.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/Address.h"
#include "lib/util/time/Timestamp.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Constants.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Storage {

bool FloppyController::isAvailable() {
    Cpu::disableInterrupts();
    Cmos::disableNmi();
    auto cmosValue = Cmos::read(0x10);
    Cmos::enableNmi();
    Cpu::enableInterrupts();

    return cmosValue != 0x00;
}

FloppyController::FloppyController() :
        statusRegisterA(IO_BASE_ADDRESS + 0), statusRegisterB(IO_BASE_ADDRESS + 1), digitalOutputRegister(IO_BASE_ADDRESS + 2),
        tapeDriveRegister(IO_BASE_ADDRESS + 3), mainStatusRegister(IO_BASE_ADDRESS + 4), dataRateSelectRegister(IO_BASE_ADDRESS + 4),
        fifoRegister(IO_BASE_ADDRESS + 5), digitalInputRegister(IO_BASE_ADDRESS + 7), configControlRegister(IO_BASE_ADDRESS + 7) {}

bool FloppyController::isBusy() {
    return (mainStatusRegister.readByte() & 0x10u) == 0x10;
}

bool FloppyController::isFifoBufferReady() {
    return (mainStatusRegister.readByte() & 0x80u) == 0x80;
}

void FloppyController::initializeAvailableDrives() {
    if (!isAvailable()) {
        LOG_INFO("No floppy drives available");
        return;
    }

    Cpu::disableInterrupts();
    Cmos::disableNmi();
    uint8_t driveInfo = Cmos::read(0x10);
    Cmos::enableNmi();
    Cpu::enableInterrupts();

    plugin();

    auto primaryDriveType = static_cast<DriveType>(driveInfo >> 4u);
    auto secondaryDriveType = static_cast<DriveType>(driveInfo & static_cast<uint8_t>(0xfu));

    if (primaryDriveType != NONE && primaryDriveType != UNKNOWN_1 && primaryDriveType != UNKNOWN_2) {
        LOG_INFO("Found primary floppy drive");

        auto *device = new FloppyDevice(*this, 0, primaryDriveType);
        auto success = resetDrive(*device);

        if (success) {
            Kernel::Service::getService<Kernel::StorageService>().registerDevice(device, DEVICE_CLASS);
        } else {
            LOG_ERROR("Unable to initializeScene primary floppy drive");
            delete device;
        }
    }

    if (secondaryDriveType != NONE && secondaryDriveType != UNKNOWN_1 && secondaryDriveType != UNKNOWN_2) {
        LOG_INFO("Found secondary floppy drive");

        auto *device = new FloppyDevice(*this, 1, secondaryDriveType);
        auto success = resetDrive(*device);

        if (success) {
            Kernel::Service::getService<Kernel::StorageService>().registerDevice(device, DEVICE_CLASS);
        } else {
            LOG_ERROR("Unable to initializeScene secondary floppy drive");
            delete device;
        }
    }
}

void FloppyController::writeFifoByte(uint8_t command) {
    uint32_t timeout = 0;
    while (timeout < TIMEOUT) {
        if (isFifoBufferReady()) {
            fifoRegister.writeByte(command);
            return;
        }

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;
    }

    LOG_ERROR("Timeout while issuing write command");
}

uint8_t FloppyController::readFifoByte() {
    uint32_t timeout = 0;
    while (timeout < TIMEOUT) {
        if (isFifoBufferReady()) {
            return fifoRegister.readByte();
        }

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;
    }

    LOG_ERROR("Timeout while reading data from FIFO-buffer");
    return 0;
}

FloppyController::SenseInterruptState FloppyController::senseInterrupt() {
    writeFifoByte(SENSE_INTERRUPT);
    return {
        readFifoByte(),
        readFifoByte()
    };
}

FloppyController::CommandStatus FloppyController::readCommandStatus() {
    return {
        readFifoByte(),
        readFifoByte(),
        readFifoByte(),
        readFifoByte(),
        readFifoByte(),
        readFifoByte(),
        readFifoByte()
    };
}

void FloppyController::setMotorState(FloppyDevice &device, FloppyController::MotorState desiredState) {
    if (desiredState == WAIT) {
        return;
    } else if (desiredState == ON) {
        if (device.getMotorState() == ON) {
            return;
        } else if (device.getMotorState() == WAIT) {
            device.setMotorState(ON);
            return;
        }

        digitalOutputRegister.writeByte(device.getDriveNumber() | static_cast<uint8_t>(0x1c)); // Turn on the floppy motor
        device.setMotorState(ON);
    } else if (desiredState == OFF) {
        if (device.getMotorState() == OFF || device.getMotorState() == WAIT) {
            return;
        }

        device.setMotorState(WAIT);
    }
}

void FloppyController::killMotor(FloppyDevice &device) {
    digitalOutputRegister.writeByte(device.getDriveNumber() | static_cast<uint8_t>(0x0c));
    device.setMotorState(OFF);
}

bool FloppyController::checkMedia(FloppyDevice &device) {
    setMotorState(device, ON);
    // Check disk change indicator flag
    if (digitalInputRegister.readByte() & 0x80) {
        // If the bit is set, a disk change has occurred and we need to check, if a disk is present
        // First we let the drive seek a cylinder other than 0 and afterwards we let it seek back to 0
        seek(device, 37, 0);
        seek(device, 0, 0);

        // If the bit is still set, there is no disk present in the drive
        // Otherwise, a disk swap has occurred
        return (digitalInputRegister.readByte() & 0x80) == 0;
    } else {
        // If bit is not set, a disk is present
        return true;
    }
}

bool FloppyController::resetDrive(FloppyDevice &device) {
    LOG_INFO("Resetting drive %u", device.getDriveNumber());
    receivedInterrupt = false;

    digitalOutputRegister.writeByte(0x00); // Disable controller;
    digitalOutputRegister.writeByte(device.getDriveNumber() | static_cast<uint8_t>(0x0c)); // Enable controller;

    uint32_t timeout = 0;
    while (!receivedInterrupt) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;

        if (timeout > TIMEOUT) {
            LOG_ERROR("Timeout while resetting drive");
            return false;
        }
    }

    senseInterrupt();

    switch (device.getDriveType()) {
        case DRIVE_360KB_5_25 :
            configControlRegister.writeByte(RATE_300K);
            break;
        case DRIVE_1200KB_5_25 :
            configControlRegister.writeByte(DataRate::RATE_500K);
            break;
        case DRIVE_720KB_3_5 :
            configControlRegister.writeByte(DataRate::RATE_250K);
            break;
        case DRIVE_1440KB_3_5 :
            configControlRegister.writeByte(DataRate::RATE_500K);
            break;
        case DRIVE_2880KB_3_5 :
            configControlRegister.writeByte(DataRate::RATE_1M);
            break;
        default :
            configControlRegister.writeByte(DataRate::RATE_500K);
            break;
    }

    writeFifoByte(SPECIFY);
    writeFifoByte(0xdf);
    writeFifoByte(0x02);

    if (device.getDriveType() == DRIVE_2880KB_3_5) {
        writeFifoByte(PERPENDICULAR_MODE);
        writeFifoByte(0x04 << device.getDriveNumber());
    }

    bool success = calibrateDrive(device);
    if (!success) {
        LOG_ERROR("Failed to reset drive %u", device.getDriveNumber());
    }

    return success;
}

bool FloppyController::calibrateDrive(FloppyDevice &device) {
    LOG_INFO("Calibrating drive %u", device.getDriveNumber());
    setMotorState(device, ON);

    for (uint8_t i = 0; i < RETRY_COUNT; i++) {
        receivedInterrupt = false;

        writeFifoByte(RECALIBRATE);
        writeFifoByte(device.getDriveNumber());

        uint32_t timeout = 0;
        while (!receivedInterrupt && timeout < TIMEOUT) {
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
            timeout += 10;
        }

        if (!receivedInterrupt) {
            continue;
        }

        SenseInterruptState interruptState = senseInterrupt();
        if (interruptState.statusRegister0 != 0x20) {
            continue;
        }

        if (interruptState.currentCylinder == 0) {
            setMotorState(device, OFF);
            return true;
        }
    }

    LOG_ERROR("Failed to calibrate drive %u", device.getDriveNumber());
    setMotorState(device, OFF);
    return false;
}

bool FloppyController::seek(FloppyDevice &device, uint8_t cylinder, uint8_t head) {
    setMotorState(device, ON);
    for (uint8_t i = 0; i < RETRY_COUNT; i++) {
        receivedInterrupt = false;

        writeFifoByte(SEEK);
        writeFifoByte(device.getDriveNumber() | static_cast<uint8_t>(head << 2u));
        writeFifoByte(cylinder);
        uint32_t timeout = 0;

        while (!receivedInterrupt && timeout < TIMEOUT) {
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
            timeout += 10;
        }

        if (!receivedInterrupt) {
            continue;
        }

        SenseInterruptState interruptState = senseInterrupt();
        if (interruptState.statusRegister0 != 0x20) {
            continue;
        }

        if (interruptState.currentCylinder == cylinder) {
            setMotorState(device, OFF);
            return true;
        }
    }

    LOG_ERROR("Failed to seek on drive %u: Did not find cylinder %u", device.getDriveNumber(), cylinder);
    setMotorState(device, OFF);
    return false;
}

void FloppyController::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::FLOPPY, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::FLOPPY);
}

void FloppyController::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    receivedInterrupt = true;
}

void FloppyController::prepareDma(FloppyDevice &device, Isa::TransferMode transferMode, void *dmaMemory, uint8_t sectorCount) {
    Isa::selectChannel(2);
    Isa::setAddress(2, static_cast<const uint8_t*>(dmaMemory));
    Isa::setCount(2, static_cast<uint16_t>(device.getSectorSize() * sectorCount - 1));
    Isa::setMode(2, transferMode, false, false, Isa::SINGLE_TRANSFER);
    Isa::deselectChannel(2);
}

bool FloppyController::performIO(FloppyDevice &device, FloppyController::TransferMode mode, uint8_t *buffer, uint8_t cylinder, uint8_t head, uint8_t startSector, uint8_t sectorCount) {
    if (cylinder >= device.getCylinders()) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "FloppyController: Trying to read/write out of cylinder bounds!");
    }

    if (head > 1) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "FloppyController: Trying to read/write out of head bounds!");
    }

    if (startSector + sectorCount - 1 > device.getSectorsPerCylinder() * 2) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "FloppyController: Trying to read/write out of track bounds!");
    }

    ioLock.acquire();
    if (!seek(device, cylinder, head)) {
        ioLock.release();
        return false;
    }

    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    const auto dmaSize = sectorCount * device.getSectorSize();
    const auto dmaPages = dmaSize % Util::PAGESIZE == 0 ? (dmaSize / Util::PAGESIZE) : (dmaSize / Util::PAGESIZE) + 1;
    void *dmaBuffer = memoryService.allocateIsaMemory(dmaPages);
    void *physicalDmaAddress = memoryService.getPhysicalAddress(dmaBuffer);
    bool success = false;

    if (mode == WRITE) {
        auto sourceAddress = Util::Address<uint32_t>(buffer);
        auto targetAddress = Util::Address<uint32_t>(dmaBuffer);
        targetAddress.copyRange(sourceAddress, sectorCount * device.getSectorSize());
    }

    setMotorState(device, ON);
    for (uint8_t i = 0; i < RETRY_COUNT; i++) {
        receivedInterrupt = false;
        prepareDma(device, mode == WRITE ? Isa::READ : Isa::WRITE, physicalDmaAddress, sectorCount);

        writeFifoByte(static_cast<uint8_t>(mode == WRITE ? WRITE_DATA : READ_DATA) | MULTITRACK | MFM);
        writeFifoByte(device.getDriveNumber() | (head << 2u));
        writeFifoByte(cylinder);
        writeFifoByte(head);
        writeFifoByte(startSector);
        writeFifoByte(2);
        writeFifoByte(device.getSectorsPerCylinder());
        writeFifoByte(device.getGapLength());
        writeFifoByte(0xff);

        uint32_t timeout = 0;
        while (!receivedInterrupt && timeout < TIMEOUT) {
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
            timeout += 10;
        }

        if (!receivedInterrupt) {
            if (!handleReadWriteError(device, cylinder, head)) {
                LOG_ERROR("Timeout while reading/writing on drive %u", device.getDriveNumber());
                break;
            }
            continue;
        }

        CommandStatus status = readCommandStatus();
        if ((status.statusRegister0 & 0xc0) != 0) {
            if (!handleReadWriteError(device, cylinder, head)) {
                LOG_ERROR("Failed to read/write on drive %u", device.getDriveNumber());
                break;
            }
            continue;
        }

        if (mode == READ) {
            auto sourceAddress = Util::Address<uint32_t>(dmaBuffer);
            auto targetAddress = Util::Address<uint32_t>(buffer);
            targetAddress.copyRange(sourceAddress, device.getSectorSize() * sectorCount);
        }

        success = true;
        break;
    }

    if (!success) {
        LOG_ERROR("Failed to read/write on drive %u", device.getDriveNumber());
    }

    setMotorState(device, OFF);
    delete reinterpret_cast<uint8_t*>(dmaBuffer);

    ioLock.release();
    return success;
}

bool FloppyController::handleReadWriteError(FloppyDevice &device, uint8_t cylinder, uint8_t head) {
    if (checkMedia(device)) {
        calibrateDrive(device);

        if (!seek(device, 0, 0)) {
            return false;
        }

        return seek(device, cylinder, head);
    }

    return false;
}

}