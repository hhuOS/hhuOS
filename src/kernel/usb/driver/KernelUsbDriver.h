#ifndef KERNEL_USB_DRIVER__INCLUDE
#define KERNEL_USB_DRIVER__INCLUDE

#include "../../../lib/util/base/String.h"

namespace Kernel::Usb::Driver{

class KernelUsbDriver{

public:

    explicit KernelUsbDriver(Util::String name);

    virtual int initialize() = 0;

    virtual int submit() = 0;

    char* getName();

private:

Util::String driver_name;

};

};

#endif