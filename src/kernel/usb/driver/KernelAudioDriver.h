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
    Util::Io::QueueInputStream* output_terminal_routine(Interface* interface, uint32_t frame_size);
    bool get_requests(const Util::Array<uint32_t>& params, struct AudioDev* audio_dev,
        uint32_t (*get_call)(AudioDriver* driver, ASInterface* as_interface));
    bool direct_requests(const Util::Array<uint32_t>& parameters,
        AudioDev* audio_dev, int8_t (*direct_req)(AudioDriver* driver, 
        AudioDev* audio_dev, Interface* as_interface));
    bool set_sound_requests(const Util::Array<uint32_t>& parameters,
        AudioDev* audio_dev, int8_t (*set_call)(AudioDriver* driver, int16_t wVolume, AudioDev* audio_dev, 
        Interface* as_interface));
    Interface* request_common_routine(
        const Util::Array<uint32_t>& parameters, AudioDev* audio_dev);
    bool get_sound_requests(const Util::Array<uint32_t>& parameters,
        AudioDev* audio_dev, uint32_t (*get_sound_call)(AudioDriver* driver, AudioDev* audio_dev));
    AudioDriver* driver; 
    bool open_audio_stream(AudioDev* audio_dev, Interface* as_streaming);
    bool close_audio_stream(AudioDev* audio_dev, Interface* as_streaming);
    bool add_transfer(AudioDev* audio_dev, Interface* itf, ASInterface* as_interface);
    bool remove_transfer(AudioDev* audio_dev, ASInterface* as_interface);
    bool set_frequency(const Util::Array<uint32_t>& parameters, AudioDev* audio_dev);
    bool audio_stream(const Util::Array<uint32_t>& parameters,
        AudioDev* audio_dev, uint32_t type);
    void clear_buffer(AudioDev* audio_dev, Interface* as_streaming);
};

};

#ifdef __cplusplus
extern "C" {
#endif

void audio_input_event_callback(uint8_t* map_io_buffer, uint16_t len, void* b);
void audio_output_event_callback(uint8_t* map_io_buffer, uint16_t len, void* b);

#ifdef __cplusplus
}
#endif

#endif