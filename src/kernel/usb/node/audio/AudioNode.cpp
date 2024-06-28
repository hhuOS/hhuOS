#include "AudioNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/Service.h"
#include "filesystem/memory/MemoryDriver.h"

Kernel::Usb::AudioNode::AudioNode(const Util::String& name, Driver::KernelAudioDriver* driver,
        Util::Io::InputStream* input_stream, uint8_t minor) :
        UsbStreamNode(name, input_stream, minor), audio_driver(driver) {}

Kernel::Usb::AudioNode::AudioNode(const Util::String& name, Driver::KernelAudioDriver* driver,
        Util::Io::OutputStream* output_stream, uint8_t minor) :
        UsbStreamNode(name, output_stream, minor), audio_driver(driver) {}

Kernel::Usb::AudioNode::AudioNode(const Util::String& name, Driver::KernelAudioDriver* driver,
        Util::Io::InputStream* input_stream, Util::Io::OutputStream* output_stream, 
        uint8_t minor) : UsbStreamNode(name, input_stream, output_stream, minor), audio_driver(driver) {}

int Kernel::Usb::AudioNode::add_file_node() {
  auto &filesystem =
      Kernel::Service::getService<Kernel::FilesystemService>().getFilesystem();
  auto &driver = filesystem.getVirtualDriver("/device");
  bool success = driver.addNode("/", (Filesystem::Memory::StreamNode*)this);

  if (!success) {
    delete this;
    return -1;
  }

  return 1;
}

bool Kernel::Usb::AudioNode::control(uint32_t request, const Util::Array<uint32_t>& parameters){
  return audio_driver->control(request, parameters, this->get_minor());
}
