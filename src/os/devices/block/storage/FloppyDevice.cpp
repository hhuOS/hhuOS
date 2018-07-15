#include "FloppyDevice.h"

FloppyDevice::FloppyDevice(FloppyController &controller, uint8_t driveNumber, FloppyController::DriveType driveType,
                           const String &name) :
        StorageDevice(name), controller(controller), driveNumber(driveNumber), driveType(driveType) {

    switch(driveType) {
        case FloppyController::DRIVE_TYPE_360KB_5_25 :
            sectorsPerTrack = 9;
        case FloppyController::DRIVE_TYPE_1200KB_5_25 :
            sectorsPerTrack = 15;
        case FloppyController::DRIVE_TYPE_720KB_3_5 :
            sectorsPerTrack = 9;
        case FloppyController::DRIVE_TYPE_1440KB_3_5 :
            sectorsPerTrack = 18;
        case FloppyController::DRIVE_TYPE_2880KB_3_5 :
            sectorsPerTrack = 36;
        default :
            sectorsPerTrack = 18;
    }

    controller.resetDrive(*this);

    controller.prepareDma(Isa::TRANSFER_MODE_READ, *this);
}

FloppyDevice::CylinderHeadSector FloppyDevice::LbaToChs(uint32_t lbaSector) {
    CylinderHeadSector ret{};

    ret.cylinder = static_cast<uint16_t>(lbaSector / (2 * sectorsPerTrack));
    ret.head = static_cast<uint16_t>((lbaSector % (2 * sectorsPerTrack)) / sectorsPerTrack);
    ret.sector = static_cast<uint16_t>((lbaSector % (2 * sectorsPerTrack)) % sectorsPerTrack * 1);

    return ret;
}

String FloppyDevice::getHardwareName() {
    switch(driveType) {
        case FloppyController::DRIVE_TYPE_360KB_5_25 :
            return "5.25\" 360KB floppy drive";
        case FloppyController::DRIVE_TYPE_1200KB_5_25 :
            return "5.25\" 1.2MB floppy drive";
        case FloppyController::DRIVE_TYPE_720KB_3_5 :
            return "3.5\" 720KB floppy drive";
        case FloppyController::DRIVE_TYPE_1440KB_3_5 :
            return "3.5\" 1.44MB floppy drive";
        case FloppyController::DRIVE_TYPE_2880KB_3_5 :
            return "3.5\" 2.88MB floppy drive";
        default :
            return "Unknown floppy drive";
    }
}

uint32_t FloppyDevice::getSectorSize() {
    return 512;
}

uint64_t FloppyDevice::getSectorCount() {
    return 0;
}

bool FloppyDevice::read(uint8_t *buff, uint32_t sector, uint32_t count) {
    return false;
}

bool FloppyDevice::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    return false;
}
