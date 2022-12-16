#include <cstdint>

#include "kernel/system/BlueScreen.h"
#include "LinearFrameBufferTerminal.h"
#include "LinearFrameBufferTerminalProvider.h"
#include "lib/util/Exception.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/memory/Address.h"
#include "lib/util/stream/FileInputStream.h"

namespace Device::Graphic {

LinearFrameBufferTerminalProvider::LinearFrameBufferTerminalProvider(Util::File::File &lfbFile, Util::Graphic::Font &font, char cursor) : lfbFile(lfbFile), font(font), cursor(cursor) {
    if (!lfbFile.exists()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "LinearFrameBufferTerminalProvider: File does not exist!");
    }

    uint8_t xBuffer[16];
    uint8_t yBuffer[16];
    uint8_t bppBuffer[16];
    uint8_t pitchBuffer[16];

    Util::Memory::Address<uint32_t>(xBuffer).setRange(0, sizeof(xBuffer));
    Util::Memory::Address<uint32_t>(yBuffer).setRange(0, sizeof(yBuffer));
    Util::Memory::Address<uint32_t>(bppBuffer).setRange(0, sizeof(bppBuffer));
    Util::Memory::Address<uint32_t>(pitchBuffer).setRange(0, sizeof(pitchBuffer));

    auto stream = Util::Stream::FileInputStream(lfbFile);
    int16_t currentChar = 0;

    while (currentChar != '\n') {
        currentChar = stream.read();
    }

    for (unsigned char &i : xBuffer) {
        currentChar = stream.read();
        if (currentChar == 'x') {
            break;
        }

        i = currentChar;
    }

    for (unsigned char & i : yBuffer) {
        currentChar = stream.read();
        if (currentChar == '@') {
            break;
        }

        i = currentChar;
    }

    for (unsigned char & i : bppBuffer) {
        currentChar = stream.read();
        if (currentChar == '\n') {
            break;
        }

        i = currentChar;
    }

    uint16_t resolutionX = Util::Memory::String::parseInt(reinterpret_cast<const char*>(xBuffer));
    uint16_t resolutionY = Util::Memory::String::parseInt(reinterpret_cast<const char*>(yBuffer));
    uint8_t colorDepth = Util::Memory::String::parseInt(reinterpret_cast<const char*>(bppBuffer));

    mode = {static_cast<uint16_t>(resolutionX / font.getCharWidth()), static_cast<uint16_t>(resolutionY / font.getCharHeight()), colorDepth, 0};
}

Util::Data::Array<LinearFrameBufferTerminalProvider::ModeInfo> LinearFrameBufferTerminalProvider::getAvailableModes() const {
    return Util::Data::Array<ModeInfo>({ mode });
}

Util::Graphic::Terminal* LinearFrameBufferTerminalProvider::initializeTerminal(const ModeInfo &modeInfo) {
    if (!lfbFile.exists()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "LinearFrameBufferTerminalProvider: File does not exist!");
    }

    auto *lfb = new Util::Graphic::LinearFrameBuffer(lfbFile, false);
    auto *terminal = new LinearFrameBufferTerminal(lfb, font, cursor);
    Kernel::BlueScreen::setLfbMode(lfb->getBuffer().get(), lfb->getResolutionX(), lfb->getResolutionY(), lfb->getColorDepth(), lfb->getPitch());

    return terminal;
}

}