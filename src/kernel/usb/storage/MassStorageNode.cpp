#include "MassStorageNode.h"
#include "../UsbNode.h"
#include <cstdint>
#include "kernel/system/System.h"
#include "kernel/service/FilesystemService.h"
#include "filesystem/memory/MemoryDriver.h"
#include "lib/util/base/String.h"

Kernel::Usb::MassStorageNode::MassStorageNode(Kernel::Usb::Driver::KernelMassStorageDriver* msd_driver, uint8_t minor, Util::String node_name) 
                                        : Kernel::Usb::UsbNode(0, minor), Filesystem::Memory::MemoryNode(node_name), driver(msd_driver){}

uint64_t Kernel::Usb::MassStorageNode::readData(uint8_t* target, uint64_t start_lba, uint64_t msd_data){
  return driver->readData(target, start_lba, msd_data, this->get_minor());
}

uint64_t Kernel::Usb::MassStorageNode::writeData(const uint8_t* source, uint64_t start_lba, uint64_t msd_data){
  return driver->writeData(source, start_lba, msd_data, this->get_minor());
}

bool Kernel::Usb::MassStorageNode::control(uint32_t request, const Util::Array<uint32_t>& parameters){
  return driver->control(request, parameters, this->get_minor());
}

int Kernel::Usb::MassStorageNode::add_file_node(Util::String node_name){
  auto &filesystem =
      Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
  auto &driver = filesystem.getVirtualDriver("/device");
  bool success = driver.addNode("/", this);

  if (!success) {
    delete this;
    return -1;
  }

  return 1;
}