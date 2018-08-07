#include "FloppyDevice.h"
#include <devices/block/FloppyMotorControlThread.h>

extern "C" {
#include <lib/libc/math.h>
}

FloppyDevice::FloppyDevice(FloppyController &controller, uint8_t driveNumber, FloppyController::DriveType driveType,
                           const String &name) : StorageDevice(name), controller(controller), driveNumber(driveNumber),
                                                 driveType(driveType) {

    switch(driveType) {
        case FloppyController::DRIVE_TYPE_360KB_5_25 :
            sectorsPerTrack = 9;
            gapLength = 32;
            size = 368640;
            hardwareName = "5.25\" 360KB floppy drive";
            break;
        case FloppyController::DRIVE_TYPE_1200KB_5_25 :
            sectorsPerTrack = 15;
            gapLength = 32;
            size = 1228800;
            hardwareName = "5.25\" 1.2MB floppy drive";
            break;
        case FloppyController::DRIVE_TYPE_720KB_3_5 :
            sectorsPerTrack = 9;
            gapLength = 27;
            size = 737280;
            hardwareName = "3.5\" 720KB floppy drive";
            break;
        case FloppyController::DRIVE_TYPE_1440KB_3_5 :
            sectorsPerTrack = 18;
            gapLength = 27;
            size = 1474560;
            hardwareName = "3.5\" 1.44MB floppy drive";
            break;
        case FloppyController::DRIVE_TYPE_2880KB_3_5 :
            sectorsPerTrack = 36;
            gapLength = 27;
            size = 2949120;
            hardwareName = "3.5\" 2.88MB floppy drive";
            break;
        default :
            sectorsPerTrack = 18;
            gapLength = 32;
            size = 1474560;
            hardwareName = "Unknown floppy drive";
            break;
    }

    motorControlThread = new FloppyMotorControlThread(*this);
    motorControlThread->start();

    bool ret = controller.resetDrive(*this);

    if(ret) {
        sectorSizeExponent = controller.calculateSectorSizeExponent(*this);
    } else {
        sectorSizeExponent = 2;
    }
}

FloppyDevice::CylinderHeadSector FloppyDevice::LbaToChs(uint32_t lbaSector) {
    CylinderHeadSector ret{};

    ret.cylinder = static_cast<uint8_t>(lbaSector / (2 * sectorsPerTrack));
    ret.head = static_cast<uint8_t>((lbaSector % (2 * sectorsPerTrack)) / sectorsPerTrack);
    ret.sector = static_cast<uint8_t>((lbaSector % (2 * sectorsPerTrack)) % sectorsPerTrack + 1);

    return ret;
}

String FloppyDevice::getHardwareName() {
    return hardwareName;
}

uint32_t FloppyDevice::getSectorSize() {
    return static_cast<uint32_t>(128 * pow(2, sectorSizeExponent));
}

uint64_t FloppyDevice::getSectorCount() {
    return size / getSectorSize();
}

bool FloppyDevice::read(uint8_t *buff, uint32_t sector, uint32_t count) {
    for(uint32_t i = 0; i < count; i++) {
        CylinderHeadSector chs = LbaToChs(sector + i);

        bool result = controller.readSector(*this, buff + i * getSectorSize(), chs.cylinder, chs.head, chs.sector);

        if(!result) {
            return false;
        }
    }

    return true;
}

bool FloppyDevice::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    for(uint32_t i = 0; i < count; i++) {
        CylinderHeadSector chs = LbaToChs(sector + i);

        bool result = controller.writeSector(*this, buff + i * getSectorSize(), chs.cylinder, chs.head, chs.sector);

        if(!result) {
            return false;
        }
    }

    return true;
}
