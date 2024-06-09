#ifndef MOUSE_NODE__INCLUDE
#define MOUSE_NODE__INCLUDE

#include "../../../device/usb/events/event/Event.h"
#include "../../../device/usb/events/event/hid/KeyBoardEvent.h"
#include "../../../lib/util/io/stream/FilterInputStream.h"
#include "../../../lib/util/base/String.h"
#include "../UsbStreamNode.h"
#include "../driver/KernelMouseDriver.h"

namespace Kernel::Usb {

class MouseNode : public UsbStreamNode {

public:

    explicit MouseNode(const Util::String& name, Driver::KernelMouseDriver* driver, 
        Util::Io::InputStream* input_stream, uint8_t minor);

    MouseNode &operator=(const MouseNode& other) = delete;

    ~MouseNode() = default;

    int add_file_node() override;

private:
    Driver::KernelMouseDriver* mouse_driver;
};

};

#endif