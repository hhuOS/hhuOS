#ifndef KERNEL_USB_MOUSE_DRIVER__INCLUDE
#define KERNEL_USB_MOUSE_DRIVER__INCLUDE

#include "KernelUsbDriver.h"
extern "C"{
#include "../../../device/usb/driver/hid/MouseDriver.h"    
}

namespace Kernel::Usb::Driver{

class KernelMouseDriver : public KernelUsbDriver{

public:

    explicit KernelMouseDriver(Util::String name);

    int initialize() override;

    int submit(uint8_t minor) override;

    void create_usb_dev_node() override;

private:

MouseDriver* driver;

};

};

#endif