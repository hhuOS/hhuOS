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

#ifndef HHUOS_FLOPPY_H
#define HHUOS_FLOPPY_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/StorageService.h>
#include <kernel/services/TimeService.h>
#include <kernel/memory/manager/IOMemoryManager.h>
#include <devices/isa/Isa.h>

class FloppyDevice;

/**
 * Driver for the floppy controller.
 *
 * The floppy controller has a 16 byte FIFO buffer, which can be used to communicate with it.
 * To issue a command, one needs to write a command byte to the buffer (see enum Commands) and afterwards write
 * the command's parameters.
 * After a command has been executed, one can read the result from the fifo buffer.
 */
class FloppyController : InterruptHandler {

    friend class FloppyDevice;
    friend class FloppyMotorControlThread;

public:

    /**
     * Possible drive types.
     */
    enum DriveType {
        DRIVE_TYPE_NONE = 0x00,
        DRIVE_TYPE_360KB_5_25 = 0x01,
        DRIVE_TYPE_1200KB_5_25 = 0x02,
        DRIVE_TYPE_720KB_3_5 = 0x03,
        DRIVE_TYPE_1440KB_3_5 = 0x04,
        DRIVE_TYPE_2880KB_3_5 = 0x05,
        DRIVE_TYPE_UNKNOWN_1 = 0x06,
        DRIVE_TYPE_UNKNOWN_2 = 0x07
    };

private:

    /**
     * Command codes, which can be sent to the floppy controller.
     */
    enum Command : uint8_t {
        COMMAND_READ_TRACK = 0x02,
        COMMAND_SPECIFY = 0x03,
        COMMAND_SENSE_DRIVE_STATUS = 0x04,
        COMMAND_WRITE_DATA = 0x05,
        COMMAND_READ_DATA = 0x06,
        COMMAND_RECALIBRATE = 0x07,
        COMMAND_SENSE_INTERRUPT = 0x08,
        COMMAND_WRITE_DELETED_DATA = 0x09,
        COMMAND_READ_ID = 0x0a,
        COMMAND_READ_DELETED_DATA = 0x0c,
        COMMAND_FORMAT_TRACK = 0x0d,
        COMMAND_DUMPREG = 0x0e,
        COMMAND_SEEK = 0x0f,
        COMMAND_VERSION = 0x10,
        COMMAND_SCAN_EQUAL = 0x11,
        COMMAND_PERPENDICULAR_MODE = 0x12,
        COMMAND_CONFIGURE = 0x13,
        COMMAND_LOCK = 0x14,
        COMMAND_VERIFY = 0x16,
        COMMAND_SCAN_LOW_OR_EQUAL = 0x19,
        COMMAND_SCAN_HIGH_OR_EQUAL = 0x1d
    };

    /**
     * Flags, that can be bitwise or-ed with a command.
     */
    enum CommandFlags : uint8_t {
        FLAG_MULTITRACK = 0x80,
        FLAG_MFM = 0x40,
        FLAG_SKIP = 0x20
    };

    /**
     * Possible states of a floppy drive's motor.
     */
    enum FloppyMotorState {
        FLOPPY_MOTOR_ON = 0x00,
        FLOPPY_MOTOR_OFF = 0x01,
        FLOPPY_MOTOR_WAIT = 0x02
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

    bool receivedInterrupt = false;

    void* dmaMemory;

    IOport statusRegisterA;
    IOport statusRegisterB;
    IOport digitalOutputRegister;
    IOport tapeDriveRegister;
    IOport mainStatusRegister;
    IOport datarateSelectRegister;
    IOport fifoRegister;
    IOport digitalInputRegister;
    IOport configControlRegister;

    StorageService *storageService;
    TimeService *timeService;

    static Logger &log;

    static const constexpr uint16_t IO_BASE_ADDRESS = 0x3f0;
    static const constexpr uint32_t FLOPPY_TIMEOUT = 2000;
    static const constexpr uint32_t FLOPPY_RETRY_COUNT = 5;

private:

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
     * NOTE: Setting the state to 'FLOPPY_MOTOR_OFF' won't result in the motor being turned off immediately.
     *       Instead the state is set to 'FLOPPY_MOTOR_WAIT' and will be turned off after a few seconds,
     *       if it has not been used in that time.
     *
     * @param device The drive
     * @param desiredState The state (FLOPPY_MOTOR_ON/FLOPPY_MOTOR_OFF
     */
    void setMotorState(FloppyDevice &device, FloppyMotorState desiredState);

    /**
     * Turn a floppy drive's motor off immediately.
     *
     * @param device The device
     */
    void killMotor(FloppyDevice &device);

    /**
     * Reset and calibrate a drive.
     *
     * @param device The device
     *
     * @return true, on success
     */
    bool resetDrive(FloppyDevice &device);

    /**
     * Calibrate a drive.
     *
     * The head will be set to CHS(0, 0, 0).
     *
     * @param device The device
     *
     * @return true, on success
     */
    bool calibrateDrive(FloppyDevice &device);

    /**
     * Get the sector size exponent of an inserted floppy disk.
     *
     * A floppy's sector size is calculated by 128 * 2 ^ exponent.
     * This is done, by issuing a read command and reading the exponent from status register 0.
     *
     * @param device The device
     *
     * @return The size exponent
     */
    uint8_t calculateSectorSizeExponent(FloppyDevice &device);

    /**
     * Seek a specific head + cylinder.
     *
     * This needs to be done before issuing a read-/write-command.
     *
     * @param device The device
     * @param cylinder The cylinder
     * @param head The head
     *
     * @return true, on success.
     */
    bool seek(FloppyDevice &device, uint8_t cylinder, uint8_t head);

    /**
     * Prepare a DMA-transfer via the ISA-bus.
     *
     * This needs to be done, before issuing a read-/write-command.
     * NOTE: If one wants to read from the floppy disk, the transfer mode needs to be set to ISA::TRANSFER_MODE_WRITE.
     *       This seems to be counter-intuitive, but makes sens, because the controller reads from the disk and
     *       WRITES to memory. For reading, the same applies vice-versa.
     *
     * @param device The device
     * @param transferMode The transfer mode (ISA::TRANSFER_MODE_READ/ISA::TRANSFER_MODE_WRITE)
     */
    void prepareDma(FloppyDevice &device, Isa::TransferMode transferMode);

    /**
     * Read a sector from a floppy. The sector needs to be specified in the CHS-format.
     *
     * @param device The device
     * @param buff The buffer to write the read data to
     * @param cylinder The cylinder
     * @param head The head
     * @param sector The sector
     *
     * @return true, on success.
     */
    bool readSector(FloppyDevice &device, uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector);

    /**
     * Write a sector to a floppy. The sector needs to be specified in the CHS-format.
     *
     * @param device The device
     * @param buff The buffer to write to the floppy
     * @param cylinder The cylinder
     * @param head The head
     * @param sector The sector
     *
     * @return true, on success.
     */
    bool writeSector(FloppyDevice &device, const uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector);

public:

    /**
     * Constructor.
     */
    FloppyController();

    /**
     * Copy-constructor.
     */
    FloppyController(const FloppyController &copy) = delete;

    /**
     * Destructor.
     */
    ~FloppyController() override;

    /**
     * Check, if the floppy controller and at least one drive is available.
     */
    static bool isAvailable();

    /**
     * Initialize the controller and the drives, that are attached to it.
     */
    void setup();

    /**
     * Enable interrupts from the floppy controller.
     */
    void plugin();

    /**
     * Overriding function from InterruptDispatcher.
     */
    void trigger(InterruptFrame &frame) override;
};

#endif
