/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_FLOPPY_H
#define HHUOS_FLOPPY_H

#include <cstdint>

#include "device/cpu/IoPort.h"
#include "device/isa/Isa.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/async/Spinlock.h"

namespace Kernel {
class Logger;
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Storage {

class FloppyDevice;

/**
 * Driver for the floppy controller.
 *
 * The floppy controller has a 16 byte FIFO buffer, which can be used to communicate with it.
 * To issue a command, one needs to write a command byte to the buffer (see enum Commands) and afterwards write
 * the command's parameters.
 * After a command has been executed, one can read the result from the fifo buffer.
 */
class FloppyController : Kernel::InterruptHandler {

public:

    /**
     * Possible drive types.
     */
    enum DriveType : uint8_t {
        NONE = 0x00,
        DRIVE_360KB_5_25 = 0x01,
        DRIVE_1200KB_5_25 = 0x02,
        DRIVE_720KB_3_5 = 0x03,
        DRIVE_1440KB_3_5 = 0x04,
        DRIVE_2880KB_3_5 = 0x05,
        UNKNOWN_1 = 0x06,
        UNKNOWN_2 = 0x07
    };

    /**
     * Possible data rates.
     */
    enum DataRate : uint8_t {
        RATE_500K = 0x00,
        RATE_300K = 0x01,
        RATE_250K = 0x02,
        RATE_1M = 0x03
    };

    /**
     * Possible states of a floppy drive's motor.
     */
    enum MotorState : uint8_t {
        ON = 0x00,
        OFF = 0x01,
        WAIT = 0x02
    };

    /**
     * TransferMode operations.
     */
    enum TransferMode : uint8_t {
        READ, WRITE
    };

    /**
     * Constructor.
     */
    FloppyController();

    /**
     * Copy Constructor.
     */
    FloppyController(const FloppyController &copy) = delete;

    /**
     * Assignment operator.
     */
    FloppyController& operator=(const FloppyController &other) = delete;

    /**
     * Destructor.
     */
    ~FloppyController() override = default;

    /**
     * Check, if the floppy controller and at least one drive is available.
     */
    static bool isAvailable();

    /**
     * Initialize the controller and the drives, that are attached to it.
     */
    void initializeAvailableDrives();

    /**
     * Turn a floppy drive's motor off immediately.
     *
     * @param device The device
     */
    void killMotor(FloppyDevice &device);

    /**
     * Read or write sectors on a floppy disk. Only sectors inside one track can be read.
     * Reading across multiple tracks requires multiple calls of this function.
     *
     * @param device The device
     * @param mode Read or write
     * @param buff The buffer to write the read data to
     * @param cylinder The cylinder
     * @param head The head
     * @param startSector The first sector
     * @param sectorCount The amount of sector to read (boundary. sectorsPerCylinder)
     * @return
     */
    bool performIO(FloppyDevice &device, TransferMode mode, uint8_t *buffer, uint8_t cylinder, uint8_t head, uint8_t startSector, uint8_t sectorCount);

    /**
     * Enable interrupts from the floppy controller.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptDispatcher.
     */
    void trigger(const Kernel::InterruptFrame &frame) override;

    static const constexpr uint32_t SECTOR_SIZE = 512;

private:

    /**
     * Command codes, which can be sent to the floppy controller.
     */
    enum Command : uint8_t {
        READ_TRACK = 0x02,
        SPECIFY = 0x03,
        SENSE_DRIVE_STATUS = 0x04,
        WRITE_DATA = 0x05,
        READ_DATA = 0x06,
        RECALIBRATE = 0x07,
        SENSE_INTERRUPT = 0x08,
        WRITE_DELETED_DATA = 0x09,
        READ_ID = 0x0a,
        READ_DELETED_DATA = 0x0c,
        FORMAT_TRACK = 0x0d,
        DUMPREG = 0x0e,
        SEEK = 0x0f,
        VERSION = 0x10,
        SCAN_EQUAL = 0x11,
        PERPENDICULAR_MODE = 0x12,
        CONFIGURE = 0x13,
        LOCK = 0x14,
        VERIFY = 0x16,
        SCAN_LOW_OR_EQUAL = 0x19,
        SCAN_HIGH_OR_EQUAL = 0x1d
    };

    /**
     * Flags, that can be bitwise or-ed with a command.
     */
    enum CommandFlags : uint8_t {
        MULTITRACK = 0x80,
        MFM = 0x40,
        SKIP = 0x20
    };

    /**
     * Controller status after an interrupt.
     */
    struct SenseInterruptState {
        uint8_t statusRegister0;
        uint8_t currentCylinder;
    };

    /**
     * Controller status after a read-/write-command.
     */
    struct CommandStatus {
        uint8_t statusRegister0;
        uint8_t statusRegister1;
        uint8_t statusRegister2;
        uint8_t currentCylinder;
        uint8_t currentHead;
        uint8_t currentSector;
        uint8_t bytesPerSector;
    };

    /**
     * Prepare a DMA-transfer via the ISA-bus.
     *
     * This needs to be done, before issuing a read-/write-command.
     * NOTE: If one wants to read from the floppy disk, the transfer mode needs to be set to ISA::WRITE.
     *       This seems to be counter-intuitive, but makes sens, because the controller reads from the disk and
     *       WRITES to memory. For reading, the same applies vice-versa.
     *
     * @param device The device
     * @param transferMode The transfer mode (ISA::READ/ISA::WRITE)
     * @param dmaMemory The virtual address of the allocated block of memory, used for the dma transfer
     * @param sectorCount The amount of sectors to read from or write to
     */
    static void prepareDma(FloppyDevice &device, Isa::TransferMode transferMode, void *dmaMemory, uint8_t sectorCount);

    /**
     * Check, if the controller is busy.
     */
    bool isBusy();

    /**
     * Check, if the controller's fifo-buffer is ready.
     */
    bool isFifoBufferReady();

    /**
     * Write a byte to the controller's fifo buffer.
     */
    void writeFifoByte(uint8_t command);

    /**
     * Read a byte from the controller's fifo buffer.
     */
    uint8_t readFifoByte();

    /**
     * Issue a 'sense interrupt' command and get the controller status.
     *
     * This needs to be done, after an interrupt has occurred (except for interrupts after read-/write-commands).
     */
    SenseInterruptState senseInterrupt();

    /**
     * Get the controller status, after a read-/write-command.
     */
    CommandStatus readCommandStatus();

    /**
     * Change the state, of floppy drive's motor.
     *
     * NOTE: Setting the state to 'OFF' won't result in the motor being turned off immediately.
     *       Instead the state is set to 'WAIT' and will be turned off after a few seconds,
     *       if it has not been used in that time.
     *
     * @param device The drive
     * @param desiredState The state (ON/OFF
     */
    void setMotorState(FloppyDevice &device, MotorState desiredState);

    /**
     * Check if a floppy disk is present inside a drive.
     *
     * @param device The device
     *
     * @return true, if a disk is present
     */
    bool checkMedia(FloppyDevice &device);

    /**
     * Reset and calibrate a drive.
     *
     * @param device The device
     *
     * @return True on success
     */
    bool resetDrive(FloppyDevice &device);

    /**
     * Calibrate a drive.
     *
     * The head will be set to CHS(0, 0, 0).
     *
     * @param device The device
     *
     * @return True on success
     */
    bool calibrateDrive(FloppyDevice &device);

    /**
     * Seek a specific head + cylinder.
     *
     * This needs to be done before issuing a read-/write-command.
     *
     * @param device The device
     * @param cylinder The cylinder
     * @param head The head
     *
     * @return True on success.
     */
    bool seek(FloppyDevice &device, uint8_t cylinder, uint8_t head);

    /**
     * Check if a disk is present and recalibrate the drive, after a read-/write-error has occurred.
     *
     * @param device The device
     * @param cylinder The cylinder, that has been accessed before the error occurred
     * @param head The head, that has been accessed before the error occurred
     *
     * @return true, if the device has been recalibrated successfully
     */
    bool handleReadWriteError(FloppyDevice &device, uint8_t cylinder, uint8_t head);

    bool receivedInterrupt = false;

    Device::IoPort statusRegisterA;
    Device::IoPort statusRegisterB;
    Device::IoPort digitalOutputRegister;
    Device::IoPort tapeDriveRegister;
    Device::IoPort mainStatusRegister;
    Device::IoPort dataRateSelectRegister;
    Device::IoPort fifoRegister;
    Device::IoPort digitalInputRegister;
    Device::IoPort configControlRegister;

    Util::Async::Spinlock ioLock;

    static Kernel::Logger log;

    static const constexpr uint16_t IO_BASE_ADDRESS = 0x3f0;
    static const constexpr uint32_t TIMEOUT = 2000;
    static const constexpr uint32_t RETRY_COUNT = 5;
    static const constexpr char *DEVICE_CLASS = "floppy";
};

}

#endif
