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

#include "FloppyDevice.h"
#include "FloppyMotorControlJob.h"
#include "kernel/system/System.h"
#include "kernel/service/JobService.h"

namespace Device::Storage {

FloppyDevice::FloppyDevice(FloppyController &controller, uint8_t driveNumber, FloppyController::DriveType driveType) :
        controller(controller), driveNumber(driveNumber), driveType(driveType) {
    switch (driveType) {
        case FloppyController::DRIVE_TYPE_360KB_5_25 :
            sectorsPerTrack = 9;
            gapLength = 32;
            size = 368640;
            break;
        case FloppyController::DRIVE_TYPE_1200KB_5_25 :
            sectorsPerTrack = 15;
            gapLength = 32;
            size = 1228800;
            break;
        case FloppyController::DRIVE_TYPE_720KB_3_5 :
            sectorsPerTrack = 9;
            gapLength = 27;
            size = 737280;
            break;
        case FloppyController::DRIVE_TYPE_1440KB_3_5 :
            sectorsPerTrack = 18;
            gapLength = 27;
            size = 1474560;
            break;
        case FloppyController::DRIVE_TYPE_2880KB_3_5 :
            sectorsPerTrack = 36;
            gapLength = 27;
            size = 2949120;
            break;
        default :
            sectorsPerTrack = 18;
            gapLength = 32;
            size = 1474560;
            break;
    }

    Kernel::System::getService<Kernel::JobService>().registerJob(new FloppyMotorControlJob(*this), Kernel::Job::Priority::LOW, Util::Time::Timestamp(2, 0));
}

FloppyDevice::CylinderHeadSector FloppyDevice::lbaToChs(uint32_t lbaSector) const {
    return {
        static_cast<uint8_t>(lbaSector / (2 * sectorsPerTrack)),
        static_cast<uint8_t>((lbaSector % (2 * sectorsPerTrack)) / sectorsPerTrack),
        static_cast<uint8_t>((lbaSector % (2 * sectorsPerTrack)) % sectorsPerTrack + 1)
    };
}

uint32_t FloppyDevice::getSectorSize() {
    // Floppy sectors always have a size of 512 Bytes
    return 512;
}

uint64_t FloppyDevice::getSectorCount() {
    return size / getSectorSize();
}

uint32_t FloppyDevice::read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    for (uint32_t i = 0; i < sectorCount; i++) {
        CylinderHeadSector chs = lbaToChs(startSector + i);

        bool result = controller.readSector(*this, buffer + i * getSectorSize(), chs.cylinder, chs.head, chs.sector);
        if (!result) {
            return i;
        }
    }

    return sectorCount;
}

uint32_t FloppyDevice::write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    for (uint32_t i = 0; i < sectorCount; i++) {
        CylinderHeadSector chs = lbaToChs(startSector + i);

        bool result = controller.writeSector(*this, buffer + i * getSectorSize(), chs.cylinder, chs.head, chs.sector);
        if (!result) {
            return i;
        }
    }

    return sectorCount;
}

uint8_t FloppyDevice::getDriveNumber() const {
    return driveNumber;
}

FloppyController::MotorState FloppyDevice::getMotorState() const {
    return motorState;
}

FloppyController::DriveType FloppyDevice::getDriveType() const {
    return driveType;
}

uint8_t FloppyDevice::getGapLength() const {
    return gapLength;
}

uint8_t FloppyDevice::getSectorsPerTrack() const {
    return sectorsPerTrack;
}

uint32_t FloppyDevice::getSize() const {
    return size;
}

FloppyController& FloppyDevice::getController() const {
    return controller;
}

void FloppyDevice::setMotorState(FloppyController::MotorState state) {
    FloppyDevice::motorState = state;
}

}