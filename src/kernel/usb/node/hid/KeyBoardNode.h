#ifndef KEY_BOARD_NODE__INCLUDE
#define KEY_BOARD_NODE__INCLUDE

#include "../../../../device/usb/events/event/Event.h"
#include "../../../../device/usb/events/event/hid/KeyBoardEvent.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/base/String.h"
#include "../UsbStreamNode.h"
#include "../../driver/hid/KernelKbdDriver.h"

namespace Kernel::Usb {

class KeyBoardNode : public UsbStreamNode {

public:
    explicit KeyBoardNode(const Util::String& s, Driver::KernelKbdDriver* driver, 
        Util::Io::InputStream* input_stream, uint8_t minor);

    KeyBoardNode &operator=(const KeyBoardNode& other) = delete;

    ~KeyBoardNode() = default;

    int add_file_node() override;

private:
    Driver::KernelKbdDriver* kbd_driver;
};

};

#endif

