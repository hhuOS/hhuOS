#include "LinearFrameBufferTerminalProvider.h"
#include "util/graphic/LinearFrameBufferTerminal.h"

namespace Device::Graphic {

LinearFrameBufferTerminalProvider::LinearFrameBufferTerminalProvider(LinearFrameBufferProvider &lfbProvider, Util::Graphic::Font &font, char cursor) :
        lfbProvider(lfbProvider), font(font), cursor(cursor), supportedModes(0) {
    auto lfbModes = lfbProvider.getAvailableModes();
    supportedModes = Util::Data::Array<ModeInfo>(lfbModes.length());

    for (uint32_t i = 0; i < lfbModes.length(); i++) {
        const auto &mode = lfbModes[i];
        supportedModes[i] = {static_cast<uint16_t>(mode.resolutionX / font.getCharWidth()),
                            static_cast<uint16_t>(mode.resolutionY / font.getCharHeight()), mode.colorDepth, mode.modeNumber};
    }
}

Util::Data::Array<LinearFrameBufferTerminalProvider::ModeInfo> LinearFrameBufferTerminalProvider::getAvailableModes() const {
    return supportedModes;
}

Util::Graphic::Terminal& LinearFrameBufferTerminalProvider::initializeTerminal(Device::Graphic::TerminalProvider::ModeInfo &modeInfo) {
    auto lfbMode = lfbProvider.searchMode(modeInfo.columns * font.getCharWidth(), modeInfo.rows * font.getCharHeight(), modeInfo.colorDepth);
    return *new Util::Graphic::LinearFrameBufferTerminal(lfbProvider.initializeLinearFrameBuffer(lfbMode), font, cursor);
}

void LinearFrameBufferTerminalProvider::destroyTerminal(Util::Graphic::Terminal &terminal) {
    lfbProvider.destroyLinearFrameBuffer(reinterpret_cast<Util::Graphic::LinearFrameBufferTerminal&>(terminal).getLinearFrameBuffer());
    delete &terminal;
}

uint32_t LinearFrameBufferTerminalProvider::getVideoMemorySize() const {
    return lfbProvider.getVideoMemorySize();
}

Util::Memory::String LinearFrameBufferTerminalProvider::getVendorName() const {
    return lfbProvider.getVendorName();
}

Util::Memory::String LinearFrameBufferTerminalProvider::getDeviceName() const {
    return lfbProvider.getDeviceName();
}

Util::Memory::String LinearFrameBufferTerminalProvider::getClassName() {
    return CLASS_NAME;
}
}