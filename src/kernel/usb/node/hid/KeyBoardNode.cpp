#include "KeyBoardNode.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/io/stream/FilterInputStream.h"
#include "lib/util/io/stream/QueueInputStream.h"

#include "filesystem/memory/MemoryDriver.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/Service.h"
#include "kernel/usb/driver/hid/KernelKbdDriver.h"

Kernel::Usb::KeyBoardNode::KeyBoardNode(const Util::String& s, Kernel::Usb::Driver::KernelKbdDriver* driver, 
  Util::Io::InputStream* input_stream, uint8_t minor) : 
    Kernel::Usb::UsbStreamNode(s, input_stream, minor), kbd_driver(driver){}

int Kernel::Usb::KeyBoardNode::add_file_node() {
  auto &filesystem =
      Kernel::Service::getService<Kernel::FilesystemService>().getFilesystem();
  auto &driver = filesystem.getVirtualDriver("/device");
  bool success = driver.addNode("/", (Filesystem::Memory::StreamNode*)this);

  if (!success) {
    delete this;
    return -1;
  }

  return 1;
}