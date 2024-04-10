#include "KernelMassStorageDriver.h"
#include "../../../lib/util/base/Exception.h"
#include "../../../lib/util/base/String.h"
#include "../../log/Logger.h"
#include "../../service/MemoryService.h"
#include "../../service/UsbService.h"
#include "../../system/System.h"
#include "../storage/MassStorageNode.h"
#include "KernelUsbDriver.h"

extern "C" {
#include "../../../device/usb/dev/UsbDevice.h"
#include "../../../device/usb/driver/UsbDriver.h"
#include "../../../device/usb/driver/storage/MassStorageDriver.h"
#include "../../../device/usb/include/UsbGeneral.h"
#include "../../../device/usb/interfaces/TimeInterface.h"
}

Kernel::Logger kernel_msd_logger = Kernel::Logger::get("KernelMsdDriver");

Kernel::Usb::Driver::KernelMassStorageDriver::KernelMassStorageDriver(
    Util::String name)
    : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelMassStorageDriver::initialize() {
  int dev_found = 0;
  Kernel::MemoryService &m =
      Kernel::System::getService<Kernel::MemoryService>();
  Kernel::UsbService &u = Kernel::System::getService<Kernel::UsbService>();

  UsbDevice_ID usbDevs[] = {
      USB_INTERFACE_INFO(MASS_STORAGE_INTERFACE, 0xFF, 0xFF), {}};
  MassStorageDriver *msd_driver =
      (MassStorageDriver *)m.allocateKernelMemory(sizeof(MassStorageDriver), 0);
  msd_driver->new_mass_storage_driver = &new_mass_storage_driver;
  msd_driver->new_mass_storage_driver(msd_driver, this->getName(), usbDevs);

  this->driver = msd_driver;

  dev_found = u.add_driver((UsbDriver *)msd_driver);
  if (dev_found == -1)
    return -1;

  if (msd_driver->configure_device(msd_driver) == -1)
    return -1;
  return 1;
}

int Kernel::Usb::Driver::KernelMassStorageDriver::submit(uint8_t minor) {
  return -1;
}

void Kernel::Usb::Driver::KernelMassStorageDriver::create_usb_dev_node() {
  MassStorageDriver *msd_driver = this->driver;
  uint8_t current_msd_node_num = 0;
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    if (msd_driver->msd_map[i] == 0)
      continue;

    Util::String node_name =
        Util::String::format("msd%u", current_msd_node_num++);
    Kernel::Usb::UsbNode *msd_node =
        new Kernel::Usb::MassStorageNode(this, i, node_name);
    msd_node->add_file_node("");
    kernel_msd_logger.info("Succesful added msd node : minor %u -> associated "
                           "with 0x%x (%s driver)...",
                           msd_node->get_minor(), msd_driver, this->getName());
  }
}

bool Kernel::Usb::Driver::KernelMassStorageDriver::control(
    uint32_t request, const Util::Array<uint32_t> &parameters, uint8_t minor) {
  MassStorageDev *msd_dev = driver->get_msd_dev_by_minor(driver, minor);
  if (msd_dev == (void *)0)
    return false;
  switch (request) {
  case GET_SIZE: {
    if (parameters.length() != 2) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [volume]");
    }
    // access_ok() behaviour should be tested in here !
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t volume = parameters[1];
    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    *user_address = driver->get_drive_size(driver, msd_dev, volume);
    return true;
  };
  case GET_BLOCK_LEN: {
    if (parameters.length() != 2) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [volume]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t volume = parameters[1];
    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    *user_address = driver->get_block_size(driver, msd_dev, volume);
    return true;
  };
  case GET_BLOCK_NUM: {
    if (parameters.length() != 2) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [volume]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t volume = parameters[1];
    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    *user_address = driver->get_block_num(driver, msd_dev, volume);
    return true;
  };
  case GET_VOLUMES: {
    if (parameters.length() != 1) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "expecting uint32_t [address]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];

    *user_address = msd_dev->volumes;
    return true;
  };
  case GET_CAPACITIES_FOUND: {
    if (parameters.length() != 2) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [volume]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t volume = parameters[1];
    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    *user_address = driver->get_capacity_count(driver, msd_dev, volume);
    return true;
  };
  case GET_INQUIRY: {
    if (parameters.length() != 4) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [address], uint32_t "
          "[volume], uint32_t [param]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t *user_address_len = (uint32_t *)(uintptr_t)parameters[1];
    uint32_t volume = parameters[2];
    uint32_t param = parameters[3];

    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    if (driver->get_inquiry_data(driver, msd_dev, volume, param,
                                 (uint8_t *)user_address,
                                 (uint8_t *)user_address_len) == -1)
      return false;
    return true;
  };
  case GET_CAPACITY: {
    if (parameters.length() != 5) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [address], uint32_t "
          "[volume], uint32_t [capacity], uint32_t [param]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t *user_address_len = (uint32_t *)(uintptr_t)parameters[1];
    uint32_t volume = parameters[2];
    uint32_t capacity = parameters[3];
    uint32_t param = parameters[4];

    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    if (driver->get_capacity_descpritor(driver, msd_dev, volume, capacity,
                                        param, (uint8_t *)user_address,
                                        (uint8_t *)user_address_len) == -1)
      return false;
    return true;
  };
  case GET_SENSE: {
    return false;
  };
  case GET_READ_CAPACITY: {
    if (parameters.length() != 4) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [address], uint32_t "
          "[volume], uint32_t [param]");
    }
    uint32_t *user_address = (uint32_t *)(uintptr_t)parameters[0];
    uint32_t *user_address_len = (uint32_t *)(uintptr_t)parameters[1];
    uint32_t volume = parameters[2];
    uint32_t param = parameters[3];

    if (!driver->is_valid_volume(driver, msd_dev, volume)) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "passed invalid volume");
    }

    if (driver->get_capacity(driver, msd_dev, volume, param,
                             (uint8_t *)user_address,
                             (uint8_t *)user_address_len) == -1)
      return false;
    return true;
  };
  case SET_CALLBACK: { // 0:15 = magic number ; 16:23 = u_tag
    // callback is stored in uint32_t which could collide with unint64_t systems
    if (parameters.length() != 2) {
      Util::Exception::throwException(
          Util::Exception::INVALID_ARGUMENT,
          "expecting uint32_t [address], uint32_t [param]");
    }
    msd_callback callback = (msd_callback)(uintptr_t)parameters[0];
    uint32_t param = parameters[1];
    uint16_t magic_number = param & 0xFFFF;
    uint8_t u_tag = (param & 0xFF0000) >> 16;
    if (driver->set_callback_msd(driver, callback, magic_number, u_tag) == -1)
      return false;
    return true;
  };
  case UNSET_CALLBACK: {
    if (parameters.length() != 1) {
      Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,
                                      "expecting uint32_t [param]");
    }
    uint32_t param = parameters[0];
    uint16_t magic_number = param & 0xFFFF;
    uint8_t u_tag = (param & 0xFF0000) >> 16;

    if (driver->unset_callback_msd(driver, magic_number, u_tag) == -1)
      return false;
    return true;
  };

  default:
    return false;
  }

  return false;
}

// magic & u_tag should be equal like in the set callback control call
uint64_t Kernel::Usb::Driver::KernelMassStorageDriver::readData(
    uint8_t *targetBuffer, uint64_t start_lba, uint64_t msd_data,
    uint8_t minor) {
  uint32_t blocks = (msd_data & 0xFFFFFFFF00000000) >> 32;
  uint8_t volume = msd_data & 0xFF;
  uint16_t magic = (msd_data & 0xFFFF00) >> 8;
  uint8_t u_tag = (msd_data & 0xFF000000) >> 24;

  return driver->read_msd(driver, targetBuffer, start_lba, blocks, magic, u_tag,
                          volume, minor);
}

uint64_t Kernel::Usb::Driver::KernelMassStorageDriver::writeData(
    const uint8_t *sourceBuffer, uint64_t start_lba, uint64_t msd_data,
    uint8_t minor) {
  uint32_t blocks = (msd_data & 0xFFFFFFFF00000000) >> 32;
  uint8_t volume = msd_data & 0xFF;
  uint16_t magic = (msd_data & 0xFFFF00) >> 8;
  uint8_t u_tag = (msd_data & 0xFF000000) >> 24;

  return driver->write_msd(driver, (uint8_t *)sourceBuffer, start_lba, blocks,
                           magic, u_tag, volume, minor);
}