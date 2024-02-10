#include "KernelMouseDriver.h"
#include "KernelUsbDriver.h"
#include "../../system/System.h"
#include "../../service/MemoryService.h"
#include "../../../lib/util/base/String.h"
#include "../../service/UsbService.h"
#include "../../log/Logger.h"
#include "../hid/MouseNode.h"

extern "C"{
#include "../../../device/usb/include/UsbGeneral.h"
#include "../../../device/usb/driver/hid/MouseDriver.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/events/listeners/hid/MouseListener.h"
#include "../../../device/usb/events/listeners/EventListener.h"
}

Kernel::Logger kernel_mouse_logger = Kernel::Logger::get("KernelMouseDriver");

Kernel::Usb::Driver::KernelMouseDriver::KernelMouseDriver(Util::String name) : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelMouseDriver::initialize(){
    int m_id = 0, dev_found = 0;
    Kernel::MemoryService& m = Kernel::System::getService<Kernel::MemoryService>();
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();

    UsbDevice_ID usbDevs[] = {
        USB_INTERFACE_INFO(HID_INTERFACE, 0xFF, INTERFACE_PROTOCOL_MOUSE),
        {}
    };
    MouseDriver* mouse_driver = (MouseDriver*)m.allocateKernelMemory(sizeof(MouseDriver), 0);

    mouse_driver->new_mouse_driver = &new_mouse_driver;
    mouse_driver->new_mouse_driver(mouse_driver, this->getName(), usbDevs);

    this->driver = mouse_driver;

    dev_found = u.add_driver((UsbDriver*)this->driver);
    if(dev_found == -1) return -1;

    MouseListener* mouse_listener = (MouseListener*)m.allocateKernelMemory(sizeof(MouseListener), 0);
    mouse_listener->new_mouse_listener = &new_mouse_listener;
    mouse_listener->new_mouse_listener(mouse_listener);
    m_id = u.register_listener((EventListener*)mouse_listener);

    if(m_id < 0) return -1;

    mouse_driver->super.listener_id = m_id;

    return 1;
}

int Kernel::Usb::Driver::KernelMouseDriver::submit(uint8_t minor){
    Kernel::UsbService& u = Kernel::System::getService<Kernel::UsbService>();
    MouseDriver* mouse_driver = this->driver;

    for(int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++){
        if(mouse_driver->mouse_map[i] == 0) continue;
        UsbDev* dev = mouse_driver->dev[i].usb_dev;
        if(dev->set_idle(dev, mouse_driver->dev[i].interface) < 0) return -1;

        u.submit_interrupt_transfer(mouse_driver->dev[i].interface, usb_rcvintpipe(mouse_driver->dev[i].endpoint_addr), mouse_driver->dev[i].priority,mouse_driver->dev[i].interval,
              mouse_driver->dev[i].buffer, mouse_driver->dev[i].buffer_size, mouse_driver->dev[i].callback);
    }
    
    return 1;
}

void Kernel::Usb::Driver::KernelMouseDriver::create_usb_dev_node(){
    MouseDriver* mouse_driver = this->driver;
    uint8_t current_mouse_node_num = 0;
    for(int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++){
        if(mouse_driver->mouse_map[i] == 0) continue;

        if(this->submit(i) != -1){
            Kernel::Usb::UsbNode* mouse_node = new Kernel::Usb::MouseNode(i);
            Util::String node_name = Util::String::format("mouse%u", current_mouse_node_num++);
            mouse_node->add_file_node(node_name);
            kernel_mouse_logger.info("Succesful added mouse node : minor %u -> associated with 0x%x (%s driver)...", mouse_node->get_minor(), this, this->getName());
        }
    }
}