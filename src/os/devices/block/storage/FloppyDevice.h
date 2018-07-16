#ifndef __VirtualDiskDrive_include__
#define __VirtualDiskDrive_include__

#include "StorageDevice.h"

#include <cstdint>
#include <devices/block/FloppyController.h>

class FloppyMotorControlThread;

/**
 * Implementation of StorageDevice for a floppy disk drive.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class FloppyDevice : public StorageDevice {

    friend class FloppyController;
    friend class FloppyMotorControlThread;

private:

    struct CylinderHeadSector {
        uint8_t cylinder;
        uint8_t head;
        uint8_t sector;
    };

    FloppyController &controller;
    uint8_t driveNumber;
    FloppyController::DriveType driveType;

    FloppyMotorControlThread *motorControlThread;

    String hardwareName;

    uint8_t gapLength;
    uint8_t sectorsPerTrack;
    uint8_t sectorSizeExponent;
    uint32_t size;

    FloppyController::FloppyMotorState motorState = FloppyController::FLOPPY_MOTOR_OFF;

    CylinderHeadSector LbaToChs(uint32_t lbaSector);

public:
    /**
     * Constructor.
     *
     * @param sectorSize The virtual size of a sector on the virtual disk
     * @param sectorCount The amount of sectors, that the virtual disk shall consist of
     * @param name The name
     */
    FloppyDevice(FloppyController &controller, uint8_t driveNumber, FloppyController::DriveType driveType,
                 const String &name);

    /**
     * Copy-constructor.
     */
    FloppyDevice(FloppyDevice &copy) = delete;

    /**
     * Destructor.
     */
    ~FloppyDevice() override = default;

    /**
     * Overriding function from StorageDevice.
     */
    String getHardwareName() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t getSectorSize() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint64_t getSectorCount() override;

    /**
     * Overriding function from StorageDevice.
     */
    bool read(uint8_t *buff, uint32_t sector, uint32_t count) override;

    /**
     * Overriding function from StorageDevice.
     */
    bool write(const uint8_t *buff, uint32_t sector, uint32_t count) override;
};

#endif