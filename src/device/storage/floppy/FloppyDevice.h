/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
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

#include <cstdint>

#include "device/storage/StorageDevice.h"
#include "FloppyController.h"

namespace Device::Storage {

class FloppyMotorControlJob;

/**
 * Implementation of StorageDevice for a floppy disk drive.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class FloppyDevice : public StorageDevice {

public:

    /**
     * Constructor.
     *
     * @param sectorSize The virtual size of a sector on the virtual disk
     * @param sectorCount The amount of sectors, that the virtual disk shall consist of
     * @param name The name
     */
    FloppyDevice(FloppyController &controller, uint8_t driveNumber, FloppyController::DriveType driveType);

    /**
     * Copy Constructor.
     */
    FloppyDevice(FloppyDevice &copy) = delete;

    /**
     * Assignment operator.
     */
    FloppyDevice& operator=(const FloppyDevice &other) = delete;

    /**
     * Destructor.
     */
    ~FloppyDevice() override = default;

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
    uint32_t read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) override;

    [[nodiscard]] uint8_t getDriveNumber() const;

    [[nodiscard]] FloppyController::MotorState getMotorState() const;

    [[nodiscard]] FloppyController::DriveType getDriveType() const;

    [[nodiscard]] uint8_t getGapLength() const;

    [[nodiscard]] uint8_t getSectorsPerTrack() const;

    [[nodiscard]] uint32_t getSize() const;

    [[nodiscard]] FloppyController& getController() const;

    void setMotorState(FloppyController::MotorState state);

private:

    struct CylinderHeadSector {
        uint8_t cylinder;
        uint8_t head;
        uint8_t sector;
    };

    /**
     * Convert an LBA sector to its CHS representation .
     *
     * @param lbaSector The sector number in LBA representation
     *
     * @return The CHS representation
     */
    [[nodiscard]] CylinderHeadSector lbaToChs(uint32_t lbaSector) const;

    FloppyController &controller;
    uint8_t driveNumber;
    FloppyController::DriveType driveType;

    uint8_t gapLength;
    uint8_t sectorsPerTrack;
    uint32_t size;

    FloppyController::MotorState motorState = FloppyController::OFF;
};

}

#endif