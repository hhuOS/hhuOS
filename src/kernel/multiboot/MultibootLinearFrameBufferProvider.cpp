#include "kernel/system/System.h"
#include "kernel/system/System.h"
#include "device/graphic/lfb/LinearFrameBufferNode.h"
#include "MultibootLinearFrameBufferProvider.h"
#include "Structure.h"
#include "kernel/service/FilesystemService.h"

namespace Kernel::Multiboot {

Kernel::Logger MultibootLinearFrameBufferProvider::log = Kernel::Logger::get("Multiboot");

MultibootLinearFrameBufferProvider::MultibootLinearFrameBufferProvider() : frameBufferInfo(Structure::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch, 0};
}

bool MultibootLinearFrameBufferProvider::isAvailable() {
    const auto &frameBufferInfo = Structure::getFrameBufferInfo();
    return frameBufferInfo.type == FRAMEBUFFER_TYPE_RGB && frameBufferInfo.bpp >= 15;
}

Util::Graphic::LinearFrameBuffer* MultibootLinearFrameBufferProvider::initializeLinearFrameBuffer(const ModeInfo &modeInfo, const Util::Memory::String &filename) {
    log.info("Checking framebuffer information, provided by the bootloader");
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "LFB mode has not been initializeAvailableDrives correctly by the bootloader!");
    }

    log.info("Framebuffer information is valid (Address: [%08x], Resolution: [%ux%u@%u]", frameBufferInfo.address, frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp);
    return new Util::Graphic::LinearFrameBuffer(reinterpret_cast<void*>(frameBufferInfo.address), frameBufferInfo.width, frameBufferInfo.height,frameBufferInfo.bpp, frameBufferInfo.pitch);
}

Util::Data::Array<MultibootLinearFrameBufferProvider::ModeInfo> MultibootLinearFrameBufferProvider::getAvailableModes() const {
    return supportedModes;
}

}