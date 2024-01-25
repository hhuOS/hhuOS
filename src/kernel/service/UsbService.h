#ifndef UsbService__include
#define UsbService__include

#include "../../lib/util/base/String.h"
#include "../../filesystem/memory/MemoryDirectoryNode.h"
#include "Service.h"
#include <cstdint>
#include "UsbService_C.h"

namespace Kernel {

class UsbService : public Kernel::Service {

public:
  UsbService();

  /**
   * Copy Constructor.
   */
  UsbService(const UsbService &other) = delete;

  /**
   * Assignment operator.
   */
  UsbService &operator=(const UsbService &other) = delete;

  int add_driver(UsbDriver *driver);

  int remove_driver(UsbDriver *driver);

  void submit_bulk_transfer(Interface *interface, unsigned int pipe,
                            uint8_t prio, void *data, unsigned int len,
                            callback_function callback);

  void submit_interrupt_transfer(Interface *interface, unsigned int pipe,
                                 uint8_t prio, uint16_t interval, void *data,
                                 unsigned int len, callback_function callback);

  void submit_control_transfer(Interface *interface, unsigned int pipe,
                               uint8_t prio, void *data, uint8_t *setup,
                               callback_function callback);

  int register_callback(uint16_t register_type, event_callback event_c);

  int deregister_callback(uint16_t register_type, event_callback event_c);

  int register_listener(EventListener *listener);

  int deregister_listener(int id);

  void create_usb_fs();

  static const constexpr uint8_t SERVICE_ID = 10;

private:
  UsbService_C *usb_service_c;
  
  void write_to_node(Filesystem::Memory::MemoryDirectoryNode* dev_node, Util::String& s, Util::String name);
};

}; // namespace Kernel

#endif