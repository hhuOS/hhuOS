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

#ifndef __VirtualDiskDrive_include__
#define __VirtualDiskDrive_include__

#include "StorageDevice.h"

#include <cstdint>
#include <devices/storage/controller/FloppyController.h>

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

    /**
     * Convert an LBA sector to its CHS representation .
     *
     * @param lbaSector The sector number in LBA representation
     *
     * @return The CHS representation
     */
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
    ~FloppyDevice() override;

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