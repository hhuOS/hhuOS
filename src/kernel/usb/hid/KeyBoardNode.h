#ifndef KEY_BOARD_NODE__INCLUDE
#define KEY_BOARD_NODE__INCLUDE

#include "../../../device/usb/events/event/Event.h"
#include "../../../device/usb/events/event/hid/KeyBoardEvent.h"
#include "../../../lib/util/io/stream/FilterInputStream.h"
#include "../../../lib/util/base/String.h"
#include "../UsbNode.h"

namespace Kernel::Usb {

class KeyBoardNode : public UsbNode, Util::Io::FilterInputStream {

public:

    explicit KeyBoardNode(uint8_t minor);

    KeyBoardNode &operator=(const KeyBoardNode& other) = delete;

    ~KeyBoardNode() = default;

    int add_file_node(Util::String node_name) override;
};

};

#ifdef __cplusplus
extern "C" {
#endif

void key_board_node_callback(void* e);

#ifdef __cplusplus
}
#endif

#endif

