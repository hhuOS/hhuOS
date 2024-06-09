#include "KernelKbdDriver.h"
#include "../../../lib/util/base/String.h"
#include "../../log/Logger.h"
#include "../../service/MemoryService.h"
#include "../../service/UsbService.h"
#include "../../system/System.h"
#include "../hid/KeyBoardNode.h"
#include "KernelUsbDriver.h"
#include "../../../lib/util/io/stream/QueueInputStream.h"
#include "../../../lib/util/collection/ArrayBlockingQueue.h"
#include "../UsbRegistry.h"

extern "C" {
#include "../../../device/usb/dev/UsbDevice.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/driver/hid/KeyBoardDriver.h"
#include "../../../device/usb/events/listeners/EventListener.h"
#include "../../../device/usb/events/listeners/hid/KeyBoardListener.h"
#include "../../../device/usb/include/UsbGeneral.h"
#include "../../../device/usb/events/event/Event.h"
#include "../../../device/usb/events/event/hid/KeyBoardEvent.h"
#include "../../../device/usb/include/UsbControllerInclude.h"
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

  __STRUCT_INIT__(kbd_driver, new_key_board_driver, new_key_board_driver,
    this->getName(), usbDevs);

  this->driver = kbd_driver;

  dev_found = u.add_driver((UsbDriver *)driver);
  __IF_RET_NEG__(__IS_NEG_ONE__(dev_found));

  KeyBoardListener *key_board_listener =
      (KeyBoardListener *)m.allocateKernelMemory(sizeof(KeyBoardListener), 0);
  __STRUCT_INIT__(key_board_listener, new_listener, new_key_board_listener);
  k_id = u.register_listener((EventListener *)key_board_listener);

  __IF_RET_NEG__(k_id < 0);

  kbd_driver->super.listener_id = k_id;

  return __RET_S__;
}

int Kernel::Usb::Driver::KernelKbdDriver::submit(uint8_t minor) {
  Kernel::UsbService &u = Kernel::System::getService<Kernel::UsbService>();

  KeyBoardDriver *kbd_driver = this->driver;
  UsbDev *dev = kbd_driver->dev[minor].usb_dev;
  __IF_RET_NEG__(__STRUCT_CALL__(dev, set_idle, kbd_driver->dev[minor].interface) < 0);

  u.submit_interrupt_transfer(
      kbd_driver->dev[minor].interface,
      usb_rcvintpipe(kbd_driver->dev[minor].endpoint_addr),
      kbd_driver->dev[minor].priority, kbd_driver->dev[minor].interval,
      kbd_driver->dev[minor].buffer, kbd_driver->dev[minor].buffer_size,
      kbd_driver->dev[minor].callback);
  return __RET_S__;
}

void Kernel::Usb::Driver::KernelKbdDriver::create_usb_dev_node() {
  KeyBoardDriver *kbd_driver = this->driver;
  uint8_t current_kbd_node_num = 0;
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    __IF_CONTINUE__(kbd_driver->key_board_map[i] == 0);

    if (__NOT_NEG_ONE__(this->submit(i))) {
      Util::String node_name =
          Util::String::format("keyboard%u", current_kbd_node_num++);
      Util::ArrayBlockingQueue<uint8_t>* keyBuffer =
        new Util::ArrayBlockingQueue<uint8_t>(BUFFER_SIZE);
      Util::Io::QueueInputStream* inputStream = new Util::Io::QueueInputStream(*keyBuffer);
      Kernel::Usb::UsbNode *kbd_node = new Kernel::Usb::KeyBoardNode(node_name, this,
        inputStream, i);

      interface_register_callback(KEY_BOARD_LISTENER, &key_board_event_callback, keyBuffer);
      
      kbd_node->add_file_node();
      kernel_kbd_logger.info("Succesful added kbd node : minor %u -> associated "
                             "with 0x%x (%s driver)...",
                             kbd_node->get_minor(), this, this->getName());
    }
  }
}

// write KeyEvent instead GenericEvent to avoid container macro
void key_board_event_callback(void *event, void* buffer) { // writes events to buffer
  Util::ArrayBlockingQueue<uint8_t>* keyBuffer = (Util::ArrayBlockingQueue<uint8_t>*)buffer;
  KeyBoardEvent *k_evt = (KeyBoardEvent *)event;
  uint8_t modifiers = k_evt->modifiers;
  uint8_t value = k_evt->super.event_value;
  uint8_t code = k_evt->super.event_code;

  keyBuffer->offer(modifiers);
  keyBuffer->offer(value);
  keyBuffer->offer(code);
}