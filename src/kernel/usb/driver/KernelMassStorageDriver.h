#ifndef KERNEL_USB_MASS_STORAGE_DRIVER__INCLUDE
#define KERNEL_USB_MASS_STORAGE_DRIVER__INCLUDE

#include "KernelUsbDriver.h"

extern "C"{
#include "../../../device/usb/driver/storage/MassStorageDriver.h"    
}

namespace Kernel::Usb::Driver{

class KernelMassStorageDriver : public KernelUsbDriver{

public:

    explicit KernelMassStorageDriver(Util::String name);

    int initialize() override;

    int submit() override;

private:
    MassStorageDriver* driver;
};

};

#endif