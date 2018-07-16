#ifndef HHUOS_FLOPPY_H
#define HHUOS_FLOPPY_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/StorageService.h>
#include <kernel/services/TimeService.h>
#include <kernel/memory/manager/IOMemoryManager.h>
#include <devices/Isa.h>

class FloppyDevice;

class FloppyController : InterruptHandler {

    friend class FloppyDevice;
    friend class FloppyMotorControlThread;

public:

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

    enum CommandFlags : uint8_t {
        FLAG_MULTITRACK = 0x80,
        FLAG_MFM = 0x40,
        FLAG_SKIP = 0x20
    };

    enum FloppyMotorState {
        FLOPPY_MOTOR_ON = 0x00,
        FLOPPY_MOTOR_OFF = 0x01,
        FLOPPY_MOTOR_WAIT = 0x02
    };

    struct SenseInterruptState {
        uint8_t statusRegister0;
        uint8_t currentCylinder;
    };

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

    IOMemInfo dmaMemInfo;

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
    static const constexpr uint32_t FLOPPY_TIMEOUT = 10000;
    static const constexpr uint32_t FLOPPY_RETRY_COUNT = 10;

private:

    bool isBusy();

    bool isFifoBufferReady();

    void writeFifoByte(uint8_t command);

    uint8_t readFifoByte();

    SenseInterruptState senseInterrupt();

    CommandStatus readCommandStatus();

    void setMotorState(FloppyDevice &device, FloppyMotorState desiredState);

    void killMotor(FloppyDevice &device);

    bool resetDrive(FloppyDevice &device);

    bool calibrateDrive(FloppyDevice &device);

    uint8_t calculateSectorSizeExponent(FloppyDevice &device);

    bool seek(FloppyDevice &device, uint8_t cylinder, uint8_t head);

    void prepareDma(FloppyDevice &device, Isa::TransferMode transferMode);

    bool readSector(FloppyDevice &device, uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector);

    bool writeSector(FloppyDevice &device, const uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector);

public:

    FloppyController();

    FloppyController(const FloppyController &copy) = delete;

    ~FloppyController() override = default;

    static bool isAvailable();

    void setup();

    void plugin();

    void trigger() override;
};

#endif
