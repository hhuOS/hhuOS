#include <kernel/Kernel.h>
#include "IODeviceManager.h"

IODeviceManager::IODeviceManager() {
    timeService = Kernel::getService<TimeService>();
}

IODeviceManager &IODeviceManager::getInstance() {
    static IODeviceManager instance;

    return instance;
}

void IODeviceManager::registerIODevice(IODevice *device) {
    lock.acquire();

    ioDevices.add(device);

    lock.release();
}

void IODeviceManager::deregisterIODevice(IODevice *device) {
    lock.acquire();

    ioDevices.remove(device);

    lock.release();
}

void IODeviceManager::run() {
    while(true) {
        lock.acquire();

        for (IODevice *device : ioDevices) {
            if (device->checkForData()) {
                InterruptFrame dummy{};

                device->trigger(dummy);
            }
        }

        lock.release();

        uint32_t timestamp = timeService->getSystemTime();

        while((timeService->getSystemTime() - timestamp) < 100) {
            yield();
        }
    }
}
