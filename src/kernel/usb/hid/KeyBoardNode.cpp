#include "filesystem/memory/StreamNode.h"
#include "filesystem/memory/MemoryDriver.h"
#include "kernel/system/System.h"
#include "../UsbRegistry.h"
#include "../../../device/usb/include/UsbControllerInclude.h"
#include "kernel/service/FilesystemService.h"
#include "../../../device/usb/events/event/Event.h"
#include "../../../device/usb/events/event/hid/KeyBoardEvent.h"
#include "../../../lib/util/io/stream/QueueInputStream.h"
#include "../../../lib/util/collection/ArrayBlockingQueue.h"
#include "../../../lib/util/io/stream/FilterInputStream.h"
#include "KeyBoardNode.h"
#include "../../../lib/util/base/String.h"

Util::ArrayBlockingQueue<uint8_t> keyBuffer = Util::ArrayBlockingQueue<uint8_t>(BUFFER_SIZE);
Util::Io::QueueInputStream inputStream = Util::Io::QueueInputStream(keyBuffer);

Kernel::Usb::KeyBoardNode::KeyBoardNode(uint8_t minor) : Kernel::Usb::UsbNode(&key_board_node_callback, minor), Util::Io::FilterInputStream(inputStream) {}

int Kernel::Usb::KeyBoardNode::add_file_node(Util::String node_name) {

  int s = interface_register_callback(KEY_BOARD_LISTENER, this->get_callback());

  if (s == -1){
    delete this;
    return -1;
  }
  
  auto *streamNode = new Filesystem::Memory::StreamNode(node_name, this);
  auto &filesystem =
      Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
  auto &driver = filesystem.getVirtualDriver("/device");
  bool success = driver.addNode("/", streamNode);

  if (!success) {
    delete streamNode;
    return -1;
  }

  return 1;
}

// write KeyEvent instead GenericEvent to avoid container macro
void key_board_node_callback(void* event){ // writes events to buffer
    KeyBoardEvent* k_evt = (KeyBoardEvent*)event;
    uint8_t modifiers = k_evt->modifiers;
    uint8_t value     = k_evt->super.event_value;
    uint8_t code      = k_evt->super.event_code;

    keyBuffer.offer(modifiers);
    keyBuffer.offer(value);
    keyBuffer.offer(code);
}