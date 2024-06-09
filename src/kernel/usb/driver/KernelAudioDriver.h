#ifndef KERNEL_USB_AUDIO_DRIVER__INCLUDE
#define KERNEL_USB_AUDIO_DRIVER__INCLUDE

#include "KernelUsbDriver.h"
#include "../audio/AudioNode.h"

extern "C"{
#include "../../../device/usb/driver/audio/AudioDeviceDriver.h"
}

namespace Kernel::Usb{
  class AudioNode;
};
namespace Util::Io{
  class QueueInputStream;
  class QueueOutputStream;
};

namespace Kernel::Usb::Driver{

class KernelAudioDriver : public KernelUsbDriver{

public:
    explicit KernelAudioDriver(Util::String name);

    int initialize() override;

    int submit(uint8_t minor) override;

    void create_usb_dev_node() override;

    bool control(uint32_t request, const Util::Array<uint32_t>& parameters, uint8_t minor);

private:
    Kernel::Usb::AudioNode* single_terminal_routine(Interface* interface, 
        Util::String name, uint8_t minor);
    Kernel::Usb::AudioNode* mult_terminal_routine(Interface** interface,
        Util::String name, uint8_t minor);
    void assign_stream(Util::Io::QueueInputStream** input_stream, 
        Util::Io::QueueOutputStream** output_stream, void* stream, uint8_t type);
    void* terminal_routine(Interface* interface, uint8_t* type);
    Util::Io::QueueOutputStream* input_terminal_routine(Interface* interface, uint32_t frame_size);
    Util::Io::QueueInputStream* output_terminal_routine(uint32_t frame_size);
    bool get_requests(const Util::Array<uint32_t>& params, struct AudioDev* audio_dev,
        uint32_t (*get_call)(AudioDriver* driver, ASInterface* as_interface));

    AudioDriver* driver;

    enum Audio_Params{
        GET_FREQ = 0x01,
        GET_FRAME_SIZE = 0x02,
        GET_SUB_FRAME_SIZE = 0x03,
        GET_BIT_DEPTH = 0x04,
        GET_TOTAL_FREQ = 0x05,
        SET_FREQ = 0x06,
        MUTE = 0x07,
        UNMUTE = 0x08,
        VOL_UP = 0x09,
        VOL_DOWN = 0x0A,
        OUT_TERMINAL_SELECT = 0x0D,
        IN_TERMINAL_SELECT = 0x0E,
    };
};

};

#ifdef __cplusplus
extern "C" {
#endif

void audio_input_event_callback(void* e, void* buffer);
void audio_output_event_callback(uint8_t* map_io_buffer, uint16_t len, void* b);

#ifdef __cplusplus
}
#endif

#endif