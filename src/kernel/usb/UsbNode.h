#ifndef USB_NODE__INCLUDE
#define USB_NODE__INCLUDE

#include <cstdint>

namespace Kernel::Usb {

class UsbNode {

public:

    explicit UsbNode(void (*r_callback)(void* e));

    virtual int add_file_node() = 0;

    ~UsbNode() = default;

    void (*get_callback())(void* e);

private:
    void (*callback)(void* e);
};

};

static const constexpr uint32_t BUFFER_SIZE = 1024;

#endif