#ifndef USB_NODE__INCLUDE
#define USB_NODE__INCLUDE

#include <cstdint>
#include "lib/util/base/String.h"

namespace Kernel::Usb {

class UsbNode {

public:
    explicit UsbNode(uint8_t minor);

    virtual int add_file_node() = 0;

    ~UsbNode() = default;

    void (*get_callback())(void* e, void* buffer);

    uint8_t get_minor();

private:
    void (*callback)(void* e, void* buffer);

    uint8_t minor;
};

};

static const constexpr uint32_t BUFFER_SIZE = 1024;

#endif