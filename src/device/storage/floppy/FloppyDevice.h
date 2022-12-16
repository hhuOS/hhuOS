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

#ifndef __VirtualDiskDrive_include__
#define __VirtualDiskDrive_include__

#include <cstdint>

#include "device/storage/StorageDevice.h"
#include "FloppyController.h"

namespace Device::Storage {

class FloppyMotorControlRunnable;

/**
 * Implementation of StorageDevice for a floppy disk drive.
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

    uint32_t performIO(FloppyController::TransferMode mode, uint8_t *buffer, uint32_t startSector, uint32_t sectorCount);

    [[nodiscard]] uint8_t getDriveNumber() const;

    [[nodiscard]] FloppyController::MotorState getMotorState() const;

    [[nodiscard]] FloppyController::DriveType getDriveType() const;

    [[nodiscard]] uint8_t getGapLength() const;

    [[nodiscard]] uint8_t getCylinders() const;

    [[nodiscard]] uint8_t getSectorsPerCylinder() const;

    void setMotorState(FloppyController::MotorState state);

    void killMotor();

private:

    FloppyController &controller;
    uint8_t driveNumber;
    FloppyController::DriveType driveType;

    uint8_t gapLength;
    uint8_t cylinders;
    uint8_t sectorsPerCylinder;

    FloppyController::MotorState motorState = FloppyController::OFF;
    FloppyMotorControlRunnable *motorControlRunnable;
};

}

#endif