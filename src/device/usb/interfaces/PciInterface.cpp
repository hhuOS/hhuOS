#include "PciInterface.h"

#include "../../../device/pci/Pci.h"
#include "../../../device/pci/PciDevice.h"
#include "../../../kernel/service/MemoryService.h"
#include "../../../kernel/system/System.h"

Pci_C *conjungt_search(Util::Array<Device::PciDevice> arr);

Pci_C *conjungt_search(Util::Array<Device::PciDevice> arr) {
  if (arr.length() == 0) return nullptr;
  int k = 0;
  Kernel::MemoryService &mem_service =
      Kernel::System::getService<Kernel::MemoryService>();

  Pci_C *pci_c = (Pci_C *)mem_service.allocateKernelMemory(sizeof(Pci_C), 0);
  PciDevice_Struct *device_structs =
      (PciDevice_Struct *)mem_service.allocateKernelMemory(
          sizeof(PciDevice_Struct) * arr.length(), 0);
  for (Device::PciDevice d : arr) {
    Device::PciDevice *pci_device_p = new Device::PciDevice(d);
    device_structs[k].new_pci_device_c = &new_pci_device_c;
    device_structs[k].new_pci_device_c(device_structs + k, pci_device_p);
    k++;
  }
  pci_c->devices_length = arr.length();
  pci_c->pci_devices = device_structs;

  return pci_c;
}

Pci_C *search_c_one_p(uint16_t vendorID) {
  Util::Array<Device::PciDevice> arr = Device::Pci::search(vendorID);
  return conjungt_search(arr);
}

Pci_C *search_c_two_p(uint16_t vendorID, uint16_t deviceID) {
  Util::Array<Device::PciDevice> arr = Device::Pci::search(vendorID, deviceID);
  return conjungt_search(arr);
}

Pci_C *search_c_controllers(uint8_t classID, uint8_t subclassID,
                            uint8_t interface) {
  Device::Pci::Class c_id = (Device::Pci::Class)classID;
  auto devices = Device::Pci::search(c_id, subclassID, interface);
  return conjungt_search(devices);
}