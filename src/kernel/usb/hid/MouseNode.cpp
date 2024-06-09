#include "MouseNode.h"
#include "../../../lib/util/base/String.h"
#include "../../../lib/util/collection/ArrayBlockingQueue.h"
#include "../../../lib/util/io/stream/FilterInputStream.h"
#include "../../../lib/util/io/stream/QueueInputStream.h"
#include "../UsbNode.h"
#include "../UsbRegistry.h"
#include "filesystem/memory/MemoryDriver.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"

Kernel::Usb::MouseNode::MouseNode(const Util::String& name, Kernel::Usb::Driver::KernelMouseDriver* driver, 
  Util::Io::InputStream* input_stream, uint8_t minor) : 
    Kernel::Usb::UsbStreamNode(name, input_stream, minor), mouse_driver(driver){}

int Kernel::Usb::MouseNode::add_file_node() {
  auto &filesystem =
      Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
  auto &driver = filesystem.getVirtualDriver("/device");
  bool success = driver.addNode("/", (Filesystem::Memory::StreamNode*)this);

  if (!success) {
    delete this;
    return -1;
  }

  return 1;
}