#ifndef KERNEL_USB_HUB_DRIVER__INCLUDE
#define KERNEL_USB_HUB_DRIVER__INCLUDE

#include "../KernelUsbDriver.h"

extern "C"{
#include "../../../../device/usb/driver/hub/HubDriver.h"
}

namespace Kernel::Usb::Driver{

class KernelHubDriver : public KernelUsbDriver{

public:

    explicit KernelHubDriver(Util::String name);

    int initialize() override;

    int submit(uint8_t minor) override;

    void create_usb_dev_node() override;

private:

    HubDriver* driver;

};

};

#endif
