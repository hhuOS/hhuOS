#ifndef HHUOS_IODEVICEMANAGER_H
#define HHUOS_IODEVICEMANAGER_H

#include <kernel/threads/Thread.h>
#include <lib/lock/Spinlock.h>
#include <kernel/services/TimeService.h>
#include <lib/util/HashSet.h>
#include "IODevice.h"

class IODeviceManager : public Thread {

private:

    TimeService *timeService = nullptr;

    Util::HashSet<IODevice*> ioDevices;

    Spinlock lock;

public:

    IODeviceManager();

    IODeviceManager(const IODeviceManager &copy) = delete;

    ~IODeviceManager() override = default;

    static IODeviceManager& getInstance();

    void registerIODevice(IODevice *device);

    void deregisterIODevice(IODevice *device);

    void run() override;

};


#endif
