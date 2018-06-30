#include "EhciDevice.h"

EhciDevice::EhciDevice(Ehci &controller, uint8_t ehciDiskNumber, String &name) : StorageDevice(name), controller(controller), ehciDiskNumber(ehciDiskNumber) {

}

bool EhciDevice::read(uint8_t *buff, uint32_t sector, uint32_t count) {
	return controller.getDevice(ehciDiskNumber)->readData(0, sector, count, buff);
}

bool EhciDevice::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    return controller.getDevice(ehciDiskNumber)->writeData(0, sector, count, (uint8_t *) buff);
}

uint32_t EhciDevice::getSectorSize() {
    // TODO: Get actual sector size from controller.
    return 512;
}

uint64_t EhciDevice::getSectorCount() {
    // TODO: Get actual sector count from controller.
    return 0;
}

String EhciDevice::getDeviceName() {
    // TODO: Get actual device name from controller.
    return "";
}
