#include "MultibootLinearFrameBufferProvider.h"
#include "Structure.h"

namespace Kernel::Multiboot {

MultibootLinearFrameBufferProvider::MultibootLinearFrameBufferProvider() : frameBufferInfo(Structure::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch, 0};
}

Util::Graphic::LinearFrameBuffer& MultibootLinearFrameBufferProvider::initializeLinearFrameBuffer(MultibootLinearFrameBufferProvider::ModeInfo &modeInfo) {
    return *new Util::Graphic::LinearFrameBuffer(frameBufferInfo.address, frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch);
}

void MultibootLinearFrameBufferProvider::destroyLinearFrameBuffer(Util::Graphic::LinearFrameBuffer &lfb) {
    delete &lfb;
}

Util::Data::Array<MultibootLinearFrameBufferProvider::ModeInfo> MultibootLinearFrameBufferProvider::getAvailableModes() const {
    return supportedModes;
}

}