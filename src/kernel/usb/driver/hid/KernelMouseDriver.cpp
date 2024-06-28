#include "KernelMouseDriver.h"
#include "lib/util/base/String.h"
#include "../../../log/Log.h"
#include "../../../service/MemoryService.h"
#include "../../../service/UsbService.h"
#include "../../../service/Service.h"
#include "../../node/hid/MouseNode.h"
#include "../KernelUsbDriver.h"
#include "lib/util/io/stream/QueueInputStream.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "../../interface/UsbRegistry.h"

extern "C" {
#include "../../../../device/usb/driver/UsbDriver.h"
#include "../../../../device/usb/driver/hid/MouseDriver.h"
#include "../../../../device/usb/events/listeners/EventListener.h"
#include "../../../../device/usb/events/listeners/hid/MouseListener.h"
#include "../../../../device/usb/include/UsbGeneral.h"
#include "../../../../device/usb/events/event/Event.h"
#include "../../../../device/usb/events/event/hid/MouseEvent.h"
#include "../../../../device/usb/include/UsbControllerInclude.h"
}

Kernel::Usb::Driver::KernelMouseDriver::KernelMouseDriver(Util::String name)
    : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelMouseDriver::initialize() {
  int m_id = 0, dev_found = 0;
  Kernel::MemoryService &m =
      Kernel::Service::getService<Kernel::MemoryService>();
  Kernel::UsbService &u = Kernel::Service::getService<Kernel::UsbService>();

  UsbDevice_ID usbDevs[] = {
      USB_INTERFACE_INFO(HID_INTERFACE, 0xFF, INTERFACE_PROTOCOL_MOUSE), {}};
  MouseDriver *mouse_driver =
      (MouseDriver *)m.allocateKernelMemory(sizeof(MouseDriver), 0);
  __STRUCT_INIT__(mouse_driver, new_mouse_driver, new_mouse_driver, this->getName(), 
    usbDevs);
  this->driver = mouse_driver;
  dev_found = u.add_driver((UsbDriver *)this->driver);
  __IF_RET_NEG__(dev_found);

  MouseListener *mouse_listener =
      (MouseListener *)m.allocateKernelMemory(sizeof(MouseListener), 0);
  __STRUCT_INIT__(mouse_listener, new_mouse_listener, new_mouse_listener);
  m_id = u.register_listener((EventListener *)mouse_listener);
  __IF_RET_NEG__(m_id < 0);

  mouse_driver->super.listener_id = m_id;

  return __RET_S__;
}

int Kernel::Usb::Driver::KernelMouseDriver::submit(uint8_t minor) {
  Kernel::UsbService &u = Kernel::Service::getService<Kernel::UsbService>();
  MouseDriver *mouse_driver = this->driver;

  UsbDev *dev = mouse_driver->dev[minor].usb_dev;
  __IF_RET_NEG__(__STRUCT_CALL__(dev, set_idle, mouse_driver->dev[minor].interface) < 0);
  u.submit_interrupt_transfer(
      mouse_driver->dev[minor].interface,
      usb_rcvintpipe(mouse_driver->dev[minor].endpoint_addr),
      mouse_driver->dev[minor].priority, mouse_driver->dev[minor].interval,
      mouse_driver->dev[minor].buffer, mouse_driver->dev[minor].buffer_size,
      mouse_driver->dev[minor].callback);

  return __RET_S__;
}

void Kernel::Usb::Driver::KernelMouseDriver::create_usb_dev_node() {
  MouseDriver *mouse_driver = this->driver;
  uint8_t current_mouse_node_num = 0;
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    __IF_CONTINUE__(__IS_ZERO__(mouse_driver->mouse_map[i]));

    if (__NOT_NEG_ONE__(this->submit(i))) {
      Util::String node_name =
          Util::String::format("mouse%u", current_mouse_node_num++);
      Util::ArrayBlockingQueue<uint8_t>* mouseBuffer =
        new Util::ArrayBlockingQueue<uint8_t>(BUFFER_SIZE);
      Util::Io::QueueInputStream* mouseinputStream =
        new Util::Io::QueueInputStream(*mouseBuffer);
      Kernel::Usb::UsbNode *mouse_node = new Kernel::Usb::MouseNode(node_name, this,
        mouseinputStream, i);
      
      interface_register_callback(MOUSE_LISTENER, mouse_event_callback, mouseBuffer);

      mouse_node->add_file_node();
      LOG_INFO("Succesful added mouse node : minor %u -> "
                               "associated with 0x%x (%s driver)...",
                               mouse_node->get_minor(), this, this->getName());
    }
  }
}

// treat mouse_movement as uint8_t instead of int8_t -> but later when
// using x,y,z we need to convert it back
void mouse_event_callback(void *e, void* b) {
  Util::ArrayBlockingQueue<uint8_t>* mouseBuffer = (Util::ArrayBlockingQueue<uint8_t>*)b;
  MouseEvent *m_event = (MouseEvent *)e;

  uint8_t value = m_event->super.event_value;
  uint8_t code = m_event->super.event_code;
  uint8_t mouse_x = m_event->x_displacement;
  uint8_t mouse_y = m_event->y_displacement;
  uint8_t mouse_z = m_event->z_displacement;

  mouseBuffer->offer(mouse_x);
  mouseBuffer->offer(mouse_y);
  mouseBuffer->offer(mouse_z);
  mouseBuffer->offer(value);
  mouseBuffer->offer(code);
}