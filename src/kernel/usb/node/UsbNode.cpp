#include "UsbNode.h"
#include "lib/util/io/stream/QueueInputStream.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/io/stream/FilterInputStream.h"
#include <cstdint>


Kernel::Usb::UsbNode::UsbNode(
    uint8_t minor) : minor(minor){}

void (*Kernel::Usb::UsbNode::get_callback())(void* e, void* buffer) { 
    return this->callback;
}

uint8_t Kernel::Usb::UsbNode::get_minor(){
    return this->minor;
}