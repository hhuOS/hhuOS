#include <lib/libc/printf.h>
#include <kernel/services/TimeService.h>
#include <kernel/Kernel.h>
#include <devices/block/storage/FloppyDevice.h>
#include <kernel/threads/WorkerThread.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/memory/SystemManagement.h>
#include "FloppyController.h"
#include "FloppyMotorControlThread.h"

extern uint8_t ___FLOPPY_START__, ___FLOPPY_END__;

Logger &FloppyController::log = Logger::get("FLOPPY");

bool FloppyController::isAvailable() {
    IOport cmosRegisterPort(0x70);
    IOport cmosDataPort(0x71);

    cmosRegisterPort.outb(0x10);

    return cmosDataPort.inb() != 0;
}

FloppyController::FloppyController() :
        statusRegisterA(IO_BASE_ADDRESS + 0), statusRegisterB(IO_BASE_ADDRESS + 1), digitalOutputRegister(IO_BASE_ADDRESS + 2),
        tapeDriveRegister(IO_BASE_ADDRESS + 3), mainStatusRegister(IO_BASE_ADDRESS + 4), datarateSelectRegister(IO_BASE_ADDRESS + 4),
        fifoRegister(IO_BASE_ADDRESS + 5), digitalInputRegister(IO_BASE_ADDRESS + 7), configControlRegister(IO_BASE_ADDRESS + 7) {

    auto dmaMemStart = reinterpret_cast<uint32_t>(VIRT2PHYS(&___FLOPPY_START__));
    auto dmaMemEnd = reinterpret_cast<uint32_t>(VIRT2PHYS(&___FLOPPY_END__));

    dmaMemInfo = SystemManagement::getInstance()->mapIO(dmaMemStart, dmaMemEnd - dmaMemStart);

    memset(reinterpret_cast<void *>(dmaMemInfo.virtStartAddress), 0, dmaMemEnd - dmaMemStart);

    timeService = Kernel::getService<TimeService>();
    storageService = Kernel::getService<StorageService>();
}

bool FloppyController::isBusy() {
    return (mainStatusRegister.inb() & 0x10u) == 0x10;
}

bool FloppyController::isFifoBufferReady() {
    return (mainStatusRegister.inb() & 0x80u) == 0x80;
}

void FloppyController::setup() {
    IOport cmosRegisterPort(0x70);
    IOport cmosDataPort(0x71);

    cmosRegisterPort.outb(0x10);

    uint8_t driveInfo = cmosDataPort.inb();

    auto primaryDriveType = static_cast<DriveType>(driveInfo >> 4u);
    auto secondaryDriveType = static_cast<DriveType>(driveInfo & static_cast<uint8_t>(0xfu));

    if(primaryDriveType != DriveType::DRIVE_TYPE_NONE && primaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_1 &&
       primaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_2) {

        log.trace("Found primary floppy drive");

        storageService->registerDevice(new FloppyDevice(*this, 0, primaryDriveType, "fdd0"));
    }

    if(secondaryDriveType != DriveType::DRIVE_TYPE_NONE && secondaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_1 &&
       secondaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_2) {

        log.trace("Found secondary floppy drive");

        storageService->registerDevice(new FloppyDevice(*this, 1, secondaryDriveType, "fdd1"));
    }
}

void FloppyController::writeFifoByte(uint8_t command) {
    uint32_t timeout = 0;

    while(timeout < FLOPPY_TIMEOUT) {
        if(isFifoBufferReady()) {
            fifoRegister.outb(command);

            return;
        }

        timeService->msleep(10);
        timeout += 10;
    }

    log.error("Timeout while issuing write command!");
}

uint8_t FloppyController::readFifoByte() {
    uint32_t timeout = 0;

    while(timeout < FLOPPY_TIMEOUT) {
        if(isFifoBufferReady()) {
            return fifoRegister.inb();
        }

        timeService->msleep(10);
        timeout += 10;
    }

    log.error("Timeout while reading data!");

    return 0;
}

FloppyController::SenseInterruptState FloppyController::senseInterrupt() {
    SenseInterruptState ret{};

    writeFifoByte(COMMAND_SENSE_INTERRUPT);

    ret.statusRegister0 = readFifoByte();
    ret.currentCylinder = readFifoByte();

    return ret;
}

FloppyController::CommandStatus FloppyController::readCommandStatus() {
    CommandStatus ret{};

    ret.statusRegister0 = readFifoByte();
    ret.statusRegister1 = readFifoByte();
    ret.statusRegister2 = readFifoByte();
    ret.currentCylinder = readFifoByte();
    ret.currentHead = readFifoByte();
    ret.currentSector = readFifoByte();
    ret.bytesPerSector = readFifoByte();

    return ret;
}

void FloppyController::setMotorState(FloppyDevice &device, FloppyController::FloppyMotorState desiredState) {
    if(desiredState == FLOPPY_MOTOR_WAIT) {
        return;
    } else if(desiredState == FLOPPY_MOTOR_ON) {
        if(device.motorState == FLOPPY_MOTOR_ON) {
            return;
        } else if(device.motorState == FLOPPY_MOTOR_WAIT) {
            device.motorState = FLOPPY_MOTOR_ON;
            device.motorControlThread->timeout = FLOPPY_TIMEOUT;

            return;
        }

        digitalOutputRegister.outb(device.driveNumber | static_cast<uint8_t>(0x1c)); // Turn on the floppy motor
        timeService->msleep(1000); // Wait a second; This should be enough time, even for older drives

        device.motorState = FLOPPY_MOTOR_ON;
    } else if(desiredState == FLOPPY_MOTOR_OFF) {
        if(device.motorState == FLOPPY_MOTOR_OFF || device.motorState == FLOPPY_MOTOR_WAIT) {
            return;
        }

        device.motorState = FLOPPY_MOTOR_WAIT;
        device.motorControlThread->timeout = FLOPPY_TIMEOUT;
    }
}

void FloppyController::killMotor(FloppyDevice &device) {
    digitalOutputRegister.outb(device.driveNumber | static_cast<uint8_t>(0x0c));
    device.motorState = FLOPPY_MOTOR_OFF;
}

bool FloppyController::resetDrive(FloppyDevice &device) {
    receivedInterrupt = false;

    digitalOutputRegister.outb(0x00); // Disable controller;
    digitalOutputRegister.outb(device.driveNumber | static_cast<uint8_t>(0x0c)); // Enable controller;

    while(!receivedInterrupt);

    senseInterrupt();

    switch(device.driveType) {
        case DRIVE_TYPE_360KB_5_25 :
            configControlRegister.outb(0x02);
            break;
        case DRIVE_TYPE_1200KB_5_25 :
            configControlRegister.outb(0x00);
            break;
        case DRIVE_TYPE_720KB_3_5 :
            configControlRegister.outb(0x01);
            break;
        case DRIVE_TYPE_1440KB_3_5 :
            configControlRegister.outb(0x00);
            break;
        case DRIVE_TYPE_2880KB_3_5 :
            configControlRegister.outb(0x00);
            break;
        default :
            configControlRegister.outb(0x00);
            break;
    }

    writeFifoByte(COMMAND_SPECIFY);
    writeFifoByte(0xdf);
    writeFifoByte(0x02);

    return calibrateDrive(device);
}

bool FloppyController::calibrateDrive(FloppyDevice &device) {
    log.trace("Calibrating drive %d", device.driveNumber);

    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        writeFifoByte(COMMAND_RECALIBRATE);
        writeFifoByte(device.driveNumber);

        while(!receivedInterrupt);
        SenseInterruptState interruptState = senseInterrupt();

        if((interruptState.statusRegister0 & 0xc0u) == 0xc0) {
            continue;
        }

        if(interruptState.currentCylinder == 0) {
            setMotorState(device, FLOPPY_MOTOR_OFF);

            log.trace("Successfully calibrated drive %d", device.driveNumber);

            return true;
        }
    }

    log.error("Failed to calibrate drive %u: Did not find cylinder 0", device.driveNumber);

    setMotorState(device, FLOPPY_MOTOR_OFF);

    return false;
}

uint8_t FloppyController::calculateSectorSizeExponent(FloppyDevice &device) {
    seek(device, 0, 0);

    setMotorState(device, FLOPPY_MOTOR_ON);

    receivedInterrupt = false;

    Isa::selectChannel(2);
    Isa::resetFlipFlop(2);
    Isa::setAddress(2, dmaMemInfo.physAddresses[0]);
    Isa::resetFlipFlop(2);
    Isa::setCount(2, 511);
    Isa::setMode(2, Isa::TRANSFER_MODE_WRITE, false, false, Isa::DMA_MODE_SINGLE_TRANSFER);
    Isa::deselectChannel(2);

    writeFifoByte(COMMAND_READ_DATA | FLAG_MULTITRACK | FLAG_MFM);
    writeFifoByte(device.driveNumber);
    writeFifoByte(0);
    writeFifoByte(device.driveNumber);
    writeFifoByte(1);
    writeFifoByte(2);
    writeFifoByte(2);
    writeFifoByte(device.gapLength);
    writeFifoByte(0xff);

    while(!receivedInterrupt);

    CommandStatus status = readCommandStatus();

    return status.bytesPerSector;
}

bool FloppyController::seek(FloppyDevice &device, uint8_t cylinder, uint8_t head) {
    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        writeFifoByte(COMMAND_SEEK);
        writeFifoByte(device.driveNumber | static_cast<uint8_t>(head << 2u));
        writeFifoByte(cylinder);

        while(!receivedInterrupt);
        SenseInterruptState interruptState = senseInterrupt();

        if((interruptState.statusRegister0 & 0xc0u) == 0xc0) {
            continue;
        }

        if(interruptState.currentCylinder == cylinder) {
            setMotorState(device, FLOPPY_MOTOR_OFF);

            return true;
        }
    }

    log.error("Failed to seek on drive %u: Did not find cylinder %u", device.driveNumber, cylinder);

    setMotorState(device, FLOPPY_MOTOR_OFF);

    return false;
}

void FloppyController::plugin() {
    IntDispatcher::getInstance().assign(IntDispatcher::floppy, *this);
    Pic::getInstance()->allow(Pic::Interrupt::FLOPPY);
}

void FloppyController::trigger() {
    receivedInterrupt = true;
}

void FloppyController::prepareDma(FloppyDevice &device, Isa::TransferMode transferMode) {
    Isa::selectChannel(2);

    Isa::resetFlipFlop(2);
    Isa::setAddress(2, dmaMemInfo.physAddresses[0]);

    Isa::resetFlipFlop(2);
    Isa::setCount(2, static_cast<uint16_t>(device.getSectorSize() - 1));

    Isa::setMode(2, transferMode, false, false, Isa::DMA_MODE_SINGLE_TRANSFER);

    Isa::deselectChannel(2);
}

bool FloppyController::readSector(FloppyDevice &device, uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector) {
    auto lastSector = static_cast<uint8_t>(sector + 1 > device.sectorsPerTrack ? device.sectorsPerTrack : sector + 1);

    seek(device, cylinder, head);

    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        prepareDma(device, Isa::TRANSFER_MODE_WRITE);

        writeFifoByte(COMMAND_READ_DATA | FLAG_MULTITRACK | FLAG_MFM);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(cylinder);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(sector);
        writeFifoByte(device.sectorSizeExponent);
        writeFifoByte(lastSector);
        writeFifoByte(device.gapLength);
        writeFifoByte(0xff);

        while(!receivedInterrupt);

        CommandStatus status = readCommandStatus();

        if((status.statusRegister0 & 0xc0u) != 0) {
            continue;
        }

        memcpy(buff, reinterpret_cast<const void *>(dmaMemInfo.virtStartAddress), device.getSectorSize());

        setMotorState(device, FLOPPY_MOTOR_OFF);

        return true;
    }

    setMotorState(device, FLOPPY_MOTOR_OFF);

    return false;
}

bool FloppyController::writeSector(FloppyDevice &device, const uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector) {
    auto lastSector = static_cast<uint8_t>(sector + 1 > device.sectorsPerTrack ? device.sectorsPerTrack : sector + 1);

    memcpy(reinterpret_cast<void *>(dmaMemInfo.virtStartAddress), buff, device.getSectorSize());

    seek(device, cylinder, head);

    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        prepareDma(device, Isa::TRANSFER_MODE_READ);

        writeFifoByte(COMMAND_WRITE_DATA | FLAG_MULTITRACK | FLAG_MFM);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(cylinder);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(sector);
        writeFifoByte(device.sectorSizeExponent);
        writeFifoByte(lastSector);
        writeFifoByte(device.gapLength);
        writeFifoByte(0xff);

        while(!receivedInterrupt);

        CommandStatus status = readCommandStatus();

        if((status.statusRegister0 & 0xc0u) != 0) {
            continue;
        }

        setMotorState(device, FLOPPY_MOTOR_OFF);

        return true;
    }

    setMotorState(device, FLOPPY_MOTOR_OFF);

    return false;
}
