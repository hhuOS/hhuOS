#include "KernelMouseDriver.h"
#include "KernelUsbDriver.h"
#include "../../system/System.h"
#include "../../service/MemoryService.h"
#include "../../../lib/util/base/String.h"
#include "../../service/UsbService.h"

extern "C"{
#include "../../../device/usb/include/UsbGeneral.h"
#include "../../../device/usb/driver/hid/MouseDriver.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/events/listeners/hid/MouseListener.h"
#include "../../../device/usb/events/listeners/EventListener.h"
}

Kernel::Usb::Driver::KernelMouseDriver::KernelMouseDriver(Util::String name) : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelMouseDriver::initialize(){
    int m_id = 0, dev_found = 0;
    Kernel::MemoryService& m = Kernel::System::getService<Kernel::MemoryService>();
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();

    MouseDriver* mouse_driver = (MouseDriver*)m.allocateKernelMemory(sizeof(MouseDriver), 0);

    MouseListener* mouse_listener = (MouseListener*)m.allocateKernelMemory(sizeof(MouseListener), 0);
    mouse_listener->new_mouse_listener = &new_mouse_listener;
    mouse_listener->new_mouse_listener(mouse_listener);
    mouse_driver->new_mouse_driver = &new_mouse_driver;
    UsbDevice_ID usbDevs[] = {
        USB_INTERFACE_INFO(HID_INTERFACE, 0xFF, INTERFACE_PROTOCOL_MOUSE),
        {}
    };
    mouse_driver->new_mouse_driver(mouse_driver, this->getName(), usbDevs);

    m_id = u.register_listener((EventListener*)mouse_listener);
    if(m_id < 0) return -1;

    mouse_driver->super.listener_id = m_id;
    this->driver = mouse_driver;

    dev_found = u.add_driver((UsbDriver*)this->driver);
    if(dev_found < 0) return -1;

    UsbDev* dev = mouse_driver->dev.usb_dev;
    if(dev->set_idle(dev, mouse_driver->dev.interface) < 0) return -1;

    u.submit_interrupt_transfer(mouse_driver->dev.interface, usb_rcvintpipe(mouse_driver->dev.endpoint_addr), mouse_driver->dev.priority,mouse_driver->dev.interval,
              mouse_driver->dev.buffer, mouse_driver->dev.buffer_size, mouse_driver->dev.callback);
    return 1;
}