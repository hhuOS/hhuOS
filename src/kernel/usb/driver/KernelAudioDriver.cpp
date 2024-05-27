#include "KernelAudioDriver.h"
#include "../../service/MemoryService.h"
#include "../../service/UsbService.h"
#include "../../system/System.h"

extern "C" {
#include "../../../device/usb/driver/audio/AudioDeviceDriver.h"
#include "../../../device/usb/include/UsbGeneral.h"
#include "../../../device/usb/dev/UsbDevice.h"
#include "../../../device/usb/driver/UsbDriver.h"
}

Kernel::Usb::Driver::KernelAudioDriver::KernelAudioDriver(Util::String name)
    : Kernel::Usb::Driver::KernelUsbDriver(name) {}

int Kernel::Usb::Driver::KernelAudioDriver::initialize() {
  int dev_found = 0;
  Kernel::MemoryService &m =
      Kernel::System::getService<Kernel::MemoryService>();
  Kernel::UsbService &u = Kernel::System::getService<Kernel::UsbService>();

  UsbDevice_ID usbDevs[] = {USB_INTERFACE_INFO(AUDIO, AUDIO_CONTROL, 0xFF),
                            USB_INTERFACE_INFO(AUDIO, AUDIO_STREAMING, 0xFF),
                            {}};

  AudioDriver *audio_driver =
      (AudioDriver*)m.allocateKernelMemory(sizeof(AudioDriver), 0);
  __STRUCT_INIT__(audio_driver, new_audio_driver, new_audio_driver, 
    this->getName(), usbDevs);

  this->driver = audio_driver;
  dev_found = u.add_driver((UsbDriver *)audio_driver);
  __IF_RET_NEG__(__IS_NEG_ONE__(dev_found));
  
  audio_driver->configure_audio_device(driver);
  return __RET_S__;
}

int Kernel::Usb::Driver::KernelAudioDriver::submit(uint8_t minor) {
  Kernel::UsbService &u = Kernel::System::getService<Kernel::UsbService>();
  Kernel::MemoryService &mem = Kernel::System::getService<Kernel::MemoryService>();

  AudioDriver* audio_driver = this->driver;
  UsbDev *dev = audio_driver->dev[minor].usb_dev;
  AudioDev audio_dev = audio_driver->dev[minor];
  __FOR_RANGE__(i, int, 0, audio_dev.audio_streaming_interfaces_num){
    ASInterface* as_interface = (ASInterface*)audio_dev.audio_streaming_interfaces[i]->active_interface->class_specific;
    if((as_interface->current_freq.sampleFreq_lo == 0) && (as_interface->current_freq.sampleFreq_hi = 0)) continue;
    uint32_t freq = (as_interface->current_freq.sampleFreq_lo | 
      (as_interface->current_freq.sampleFreq_hi << 16));
    struct Type1_FormatTypeDescriptor* type1 = (struct Type1_FormatTypeDescriptor*)(as_interface->format_type->type_descriptor);
    uint8_t* buffer = (uint8_t*)mem.mapIO(type1->bSubframeSize * freq);
    as_interface->buffer = buffer;
    as_interface->buffer_size = type1->bSubframeSize * freq;
    u.submit_iso_transfer(
      audio_dev.audio_streaming_interfaces[i],
      usb_rcvisopipe(dev->__endpoint_number(dev, dev->__get_first_endpoint(dev,
        audio_dev.audio_streaming_interfaces[i]->active_interface))),
      audio_driver->dev[minor].priority, 1,
      as_interface->buffer, as_interface->buffer_size,
      audio_driver->dev[minor].callback);
  }
  
  return __RET_S__;
}

void Kernel::Usb::Driver::KernelAudioDriver::create_usb_dev_node() {
  AudioDriver* driver = this->driver;
  __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){
    __IF_CONTINUE__(driver->audio_map[i] == 0);
    this->submit(i);
  }
}