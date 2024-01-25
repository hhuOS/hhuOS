#ifndef KERNEL_USB_KBD_DRIVER__INCLUDE
#define KERNEL_USB_KBD_DRIVER__INCLUDE

#include "KernelUsbDriver.h"

extern "C"{
#include "../../../device/usb/driver/hid/KeyBoardDriver.h"    
}

namespace Kernel::Usb::Driver{

class KernelKbdDriver : public KernelUsbDriver{

public:

    explicit KernelKbdDriver(Util::String name);

    int initialize() override;

    int submit() override;

private:

KeyBoardDriver* driver;

};

};

#endif