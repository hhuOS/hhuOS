#include "KernelKbdDriver.h"
#include "../../../lib/util/base/String.h"
#include "../../log/Logger.h"
#include "../../service/MemoryService.h"
#include "../../service/UsbService.h"
#include "../../system/System.h"
#include "../hid/KeyBoardNode.h"
#include "KernelUsbDriver.h"

extern "C" {
#include "../../../device/usb/dev/UsbDevice.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/driver/hid/KeyBoardDriver.h"
#include "../../../device/usb/events/listeners/EventListener.h"
#include "../../../device/usb/events/listeners/hid/KeyBoardListener.h"
#include "../../../device/usb/include/UsbGeneral.h"
}

Kernel::Logger kernel_kbd_logger = Kernel::Logger::get("KernelKbdDriver");

Kernel::Usb::Driver::KernelKbdDriver::KernelKbdDriver(Util::String name)
    : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelKbdDriver::initialize() {
  int k_id = 0, dev_found = 0;
  Kernel::MemoryService &m =
      Kernel::System::getService<Kernel::MemoryService>();
  Kernel::UsbService &u = Kernel::System::getService<Kernel::UsbService>();

  UsbDevice_ID usbDevs[] = {
      USB_INTERFACE_INFO(HID_INTERFACE, 0xFF, INTERFACE_PROTOCOL_KDB), {}};
  KeyBoardDriver *kbd_driver =
      (KeyBoardDriver *)m.allocateKernelMemory(sizeof(KeyBoardDriver), 0);

  kbd_driver->new_key_board_driver = &new_key_board_driver;
  kbd_driver->new_key_board_driver(kbd_driver, this->getName(), usbDevs);

  this->driver = kbd_driver;

  dev_found = u.add_driver((UsbDriver *)driver);
  if (dev_found == -1)
    return -1;

  KeyBoardListener *key_board_listener =
      (KeyBoardListener *)m.allocateKernelMemory(sizeof(KeyBoardListener), 0);
  key_board_listener->new_listener = &new_key_board_listener;
  key_board_listener->new_listener(key_board_listener);
  k_id = u.register_listener((EventListener *)key_board_listener);

  if (k_id < 0)
    return -1;

  kbd_driver->super.listener_id = k_id;

  return 1;
}

int Kernel::Usb::Driver::KernelKbdDriver::submit(uint8_t minor) {
  Kernel::UsbService &u = Kernel::System::getService<Kernel::UsbService>();

  KeyBoardDriver *kbd_driver = this->driver;
  UsbDev *dev = kbd_driver->dev[minor].usb_dev;
  if (dev->set_idle(dev, kbd_driver->dev[minor].interface) < 0)
    return -1;

  u.submit_interrupt_transfer(
      kbd_driver->dev[minor].interface,
      usb_rcvintpipe(kbd_driver->dev[minor].endpoint_addr),
      kbd_driver->dev[minor].priority, kbd_driver->dev[minor].interval,
      kbd_driver->dev[minor].buffer, kbd_driver->dev[minor].buffer_size,
      kbd_driver->dev[minor].callback);
  return 1;
}

void Kernel::Usb::Driver::KernelKbdDriver::create_usb_dev_node() {
  KeyBoardDriver *kbd_driver = this->driver;
  uint8_t current_kbd_node_num = 0;
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    if (kbd_driver->key_board_map[i] == 0)
      continue;

    if (this->submit(i) != -1) {
      Kernel::Usb::UsbNode *kbd_node = new Kernel::Usb::KeyBoardNode(i);
      Util::String node_name =
          Util::String::format("keyboard%u", current_kbd_node_num++);
      kbd_node->add_file_node(node_name);
      kernel_kbd_logger.info("Succesful added kbd node : minor %u -> associated "
                             "with 0x%x (%s driver)...",
                             kbd_node->get_minor(), this, this->getName());
    }
  }
}