#include "AhciDevice.h"

AhciDevice::AhciDevice(Ahci &controller, uint8_t ahciDiskNumber, String name) :
        StorageDevice(name), controller(controller), ahciDiskNumber(ahciDiskNumber) {
    deviceInfo = controller.getDeviceInfo(ahciDiskNumber);
}

bool AhciDevice::read(uint8_t *buff, uint32_t sector, uint32_t count) {
	return controller.read(ahciDiskNumber, sector, 0, count, (uint16_t *) buff);
}

bool AhciDevice::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    return controller.write(ahciDiskNumber, sector, 0, count, (uint16_t *) buff);
}

String AhciDevice::getDeviceName() {
    return deviceInfo.name;
}

uint32_t AhciDevice::getSectorSize() {
    // TODO: Get actual sector size from controller.
    return 512;
}

uint64_t AhciDevice::getSectorCount() {
    return deviceInfo.sectorCount;
}