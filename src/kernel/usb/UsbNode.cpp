#include "UsbNode.h"
#include "../../lib/util/io/stream/QueueInputStream.h"
#include "../../lib/util/collection/ArrayBlockingQueue.h"
#include "../../lib/util/io/stream/FilterInputStream.h"


Kernel::Usb::UsbNode::UsbNode(void (*r_callback)(void *e)) : callback(r_callback) {}

void (*Kernel::Usb::UsbNode::get_callback())(void* e) { 
    return this->callback;
}
