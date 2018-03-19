#ifndef __StorageAddEvent_include__
#define __StorageAddEvent_include__

#include "kernel/events/Event.h"
#include "devices/block/storage/StorageDevice.h"

class StorageAddEvent : public Event {

public:

    StorageAddEvent();

    explicit StorageAddEvent(StorageDevice *device);

    StorageAddEvent(const StorageAddEvent &other);

    char* getName() override;

    StorageDevice* getDevice();

    static const uint32_t TYPE   = 0x00000002;

private:

    StorageDevice *device;

};


#endif
