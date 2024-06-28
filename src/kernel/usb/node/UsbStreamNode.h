#ifndef USB_STREAM_NODE__INCLUDE
#define USB_STREAM_NODE__INCLUDE

#include "UsbNode.h"
#include "filesystem/memory/StreamNode.h"

#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/OutputStream.h"

namespace Kernel::Usb{

class UsbStreamNode : public UsbNode, Filesystem::Memory::StreamNode{

public: 
    UsbStreamNode(const Util::String& name, Util::Io::InputStream* input_stream, uint8_t minor);
    UsbStreamNode(const Util::String& name, Util::Io::OutputStream* output_stream, uint8_t minor);
    UsbStreamNode(const Util::String& name, Util::Io::InputStream* input_stream, 
        Util::Io::OutputStream* output_stream, uint8_t minor);

};

};

#endif