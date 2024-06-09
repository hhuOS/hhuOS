#include "UsbStreamNode.h"

Kernel::Usb::UsbStreamNode::UsbStreamNode(const Util::String& name, 
    Util::Io::InputStream* input_stream, uint8_t minor) : UsbNode(minor), 
        Filesystem::Memory::StreamNode(name, input_stream) {}

Kernel::Usb::UsbStreamNode::UsbStreamNode(const Util::String& name,
    Util::Io::OutputStream* output_stream, uint8_t minor) : UsbNode(minor),
        Filesystem::Memory::StreamNode(name, output_stream) {}

Kernel::Usb::UsbStreamNode::UsbStreamNode(const Util::String& name, Util::Io::InputStream* input_stream, 
        Util::Io::OutputStream* output_stream, uint8_t minor) : UsbNode(minor),
        Filesystem::Memory::StreamNode(name, output_stream, input_stream) {}
