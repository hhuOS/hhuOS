#ifndef MOUSE_NODE__INCLUDE
#define MOUSE_NODE__INCLUDE

#include "../../../device/usb/events/event/Event.h"
#include "../../../device/usb/events/event/hid/KeyBoardEvent.h"
#include "../../../lib/util/io/stream/FilterInputStream.h"
#include "../UsbNode.h"

namespace Kernel::Usb {

class MouseNode : UsbNode , Util::Io::FilterInputStream{

public:

    explicit MouseNode();

    MouseNode &operator=(const MouseNode& other) = delete;

    ~MouseNode() = default;

    int add_file_node() override;
};

};

#ifdef __cplusplus
extern "C" {
#endif

void mouse_node_callback(void* e);

#ifdef __cplusplus
}
#endif

#endif