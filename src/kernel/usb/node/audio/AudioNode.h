#ifndef AUDIO_NODE__INCLUDE
#define AUDIO_NODE__INCLUDE

#include "../UsbStreamNode.h"
#include "../../driver/audio/KernelAudioDriver.h"

namespace Kernel::Usb::Driver{
  class KernelAudioDriver;
};

namespace Kernel::Usb{

class AudioNode : public UsbStreamNode {

public:
    AudioNode(const Util::String& s, Driver::KernelAudioDriver* driver,
        Util::Io::InputStream* input_stream, uint8_t minor);
    
    AudioNode(const Util::String& s, Driver::KernelAudioDriver* driver,
        Util::Io::OutputStream* output_stream, uint8_t minor);

    AudioNode(const Util::String& s, Driver::KernelAudioDriver* driver,
        Util::Io::InputStream* input_stream, Util::Io::OutputStream* output_stream, 
        uint8_t minor); 

    int add_file_node() override;
    bool control(uint32_t request, const Util::Array<uint32_t>& parameters) override;

private:
    Driver::KernelAudioDriver* audio_driver;
};

};

#endif