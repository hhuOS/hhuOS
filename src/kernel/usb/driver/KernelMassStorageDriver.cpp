#include "KernelMassStorageDriver.h"
#include "KernelUsbDriver.h"
#include "../../system/System.h"
#include "../../service/MemoryService.h"
#include "../../../lib/util/base/String.h"
#include "../../service/UsbService.h"

extern "C"{
#include "../../../device/usb/dev/UsbDevice.h"    
#include "../../../device/usb/driver/storage/MassStorageDriver.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/include/UsbGeneral.h"
}

Kernel::Usb::Driver::KernelMassStorageDriver::KernelMassStorageDriver(Util::String name) : Kernel::Usb::Driver::KernelUsbDriver(name){}

int Kernel::Usb::Driver::KernelMassStorageDriver::initialize(){
    int dev_found = 0;
    Kernel::MemoryService& m = Kernel::System::getService<Kernel::MemoryService>();
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();

    MassStorageDriver* msd_driver = (MassStorageDriver*)m.allocateKernelMemory(sizeof(MassStorageDriver), 0);
    msd_driver->new_mass_storage_driver = &new_mass_storage_driver;
    UsbDevice_ID usbDevs[] = {
        USB_INTERFACE_INFO(MASS_STORAGE_INTERFACE, 0xFF, 0xFF),
        {}
    };

    msd_driver->new_mass_storage_driver(msd_driver, this->getName(), usbDevs);

    dev_found = u.add_driver((UsbDriver*)msd_driver);
    if(dev_found < 0) return -1;

    if(msd_driver->configure_device(msd_driver) == -1)
        return -1;
    return 1;        
}

int Kernel::Usb::Driver::KernelMassStorageDriver::submit(){
    return 1;
}