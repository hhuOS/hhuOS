#include "KernelUsbDriver.h"
#include "KernelHubDriver.h"
#include "KernelUsbDriver.h"
#include "../../system/System.h"
#include "../../service/MemoryService.h"
#include "../../../lib/util/base/String.h"
#include "../../service/UsbService.h"

extern "C"{
#include "../../../device/usb/driver/hub/HubDriver.h"
}

Kernel::Usb::Driver::KernelHubDriver::KernelHubDriver(Util::String name) : Kernel::Usb::Driver::KernelUsbDriver(name){}

int Kernel::Usb::Driver::KernelHubDriver::initialize(){
    int dev_found = 0;
    Kernel::MemoryService& m = Kernel::System::getService<Kernel::MemoryService>();
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();

    UsbDevice_ID usbDevs[] = {
      USB_DEVICE_INFO(HUB, 0xFF, 0xFF), USB_INTERFACE_INFO(HUB, 0xFF, 0xFF), {}
    };

    HubDriver* hub_driver = (HubDriver*)m.allocateKernelMemory(sizeof(HubDriver), 0);
    hub_driver->new_hub_driver = &new_hub_driver;
    hub_driver->new_hub_driver(hub_driver, this->getName(), usbDevs);
    
    dev_found = u.add_driver((UsbDriver*)hub_driver);
    if(dev_found == -1) return -1;

    if(hub_driver->configure_hub(hub_driver) == -1)
        return -1;
    return 1;
}

int Kernel::Usb::Driver::KernelHubDriver::submit(){
    return -1;
}