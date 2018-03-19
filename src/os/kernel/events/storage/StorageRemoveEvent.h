#ifndef __StorageRemoveEvent_include__
#define __StorageRemoveEvent_include__

#include "kernel/events/Event.h"
#include "devices/block/storage/StorageDevice.h"

class StorageRemoveEvent : public Event {

public:

    StorageRemoveEvent();

    explicit StorageRemoveEvent(String deviceName);

    StorageRemoveEvent(const StorageRemoveEvent &other);

    char* getName() override;

    String getDeviceName();

    static const uint32_t TYPE   = 0x00000003;

private:

    String deviceName;

};


#endif
