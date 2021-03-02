#include <kernel/core/Management.h>
#include "MultibootLinearFrameBufferProvider.h"
#include "Structure.h"

namespace Kernel::Multiboot {

MultibootLinearFrameBufferProvider::MultibootLinearFrameBufferProvider() : frameBufferInfo(Structure::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch, 0};
}

Util::Graphic::LinearFrameBuffer& MultibootLinearFrameBufferProvider::initializeLinearFrameBuffer(MultibootLinearFrameBufferProvider::ModeInfo &modeInfo) {
    // Map frame buffer into IO memory
    auto frameBuffer = Kernel::Management::getInstance().mapIO(frameBufferInfo.address, frameBufferInfo.pitch * frameBufferInfo.height);
    return *new Util::Graphic::LinearFrameBuffer(frameBuffer, frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch);
}

void MultibootLinearFrameBufferProvider::destroyLinearFrameBuffer(Util::Graphic::LinearFrameBuffer &lfb) {
    delete &lfb;
}

Util::Data::Array<MultibootLinearFrameBufferProvider::ModeInfo> MultibootLinearFrameBufferProvider::getAvailableModes() const {
    return supportedModes;
}

Util::Memory::String MultibootLinearFrameBufferProvider::getClassName() {
    return CLASS_NAME;
}

}