#include "KernelKbdDriver.h"
#include "KernelUsbDriver.h"
#include "../../system/System.h"
#include "../../service/MemoryService.h"
#include "../../../lib/util/base/String.h"
#include "../../service/UsbService.h"

extern "C"{
#include "../../../device/usb/dev/UsbDevice.h"    
#include "../../../device/usb/driver/hid/KeyBoardDriver.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/events/listeners/hid/KeyBoardListener.h"
#include "../../../device/usb/events/listeners/EventListener.h"
#include "../../../device/usb/include/UsbGeneral.h"
}

Kernel::Usb::Driver::KernelKbdDriver::KernelKbdDriver(Util::String name) : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelKbdDriver::initialize(){
    int k_id = 0, dev_found = 0;
    Kernel::MemoryService& m = Kernel::System::getService<Kernel::MemoryService>();
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();

    KeyBoardDriver* kbd_driver = (KeyBoardDriver*)m.allocateKernelMemory(sizeof(KeyBoardDriver), 0);

    KeyBoardListener* key_board_listener = (KeyBoardListener*)m.allocateKernelMemory(sizeof(KeyBoardListener), 0);
    key_board_listener->new_listener = &new_key_board_listener;
    key_board_listener->new_listener(key_board_listener);
    kbd_driver->new_key_board_driver = &new_key_board_driver;
    UsbDevice_ID usbDevs[] = {
        USB_INTERFACE_INFO(HID_INTERFACE, 0xFF, INTERFACE_PROTOCOL_KDB),
        {}
    };
    kbd_driver->new_key_board_driver(kbd_driver, this->getName(), usbDevs);

    k_id = u.register_listener((EventListener*)key_board_listener);
    if(k_id < 0) return -1;

    kbd_driver->super.listener_id = k_id;
    this->driver = kbd_driver;

    dev_found = u.add_driver((UsbDriver*)driver);
    if(dev_found < 0) return -1;

    return 1;
}

int Kernel::Usb::Driver::KernelKbdDriver::submit(){
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();

    KeyBoardDriver* kbd_driver = this->driver;
    UsbDev* dev = kbd_driver->dev.usb_dev;
    if(dev->set_idle(dev, kbd_driver->dev.interface) < 0) return -1;
    //kbd_driver->trigger_led_report(kbd_driver); -> assuming default state
    
    u.submit_interrupt_transfer(kbd_driver->dev.interface, usb_rcvintpipe(kbd_driver->dev.endpoint_addr),
              kbd_driver->dev.priority, kbd_driver->dev.interval, kbd_driver->dev.buffer,
              kbd_driver->dev.buffer_size, kbd_driver->dev.callback);
    return 1;
}
