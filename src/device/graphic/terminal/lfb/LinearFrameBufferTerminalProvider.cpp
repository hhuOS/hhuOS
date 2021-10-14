#include <lib/util/file/File.h>
#include <lib/util/stream/FileInputStream.h>
#include <kernel/service/FilesystemService.h>
#include <kernel/core/System.h>
#include <device/graphic/terminal/TerminalNode.h>
#include "LinearFrameBufferTerminalProvider.h"
#include "LinearFrameBufferTerminal.h"

namespace Device::Graphic {

LinearFrameBufferTerminalProvider::LinearFrameBufferTerminalProvider(LinearFrameBufferProvider &lfbProvider, Util::Graphic::Font &font) :
        lfbProvider(lfbProvider), font(font), supportedModes(0) {
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

bool LinearFrameBufferTerminalProvider::initializeTerminal(Device::Graphic::TerminalProvider::ModeInfo &modeInfo, const Util::Memory::String &filename) {
    auto lfbMode = lfbProvider.searchMode(modeInfo.columns * font.getCharWidth(), modeInfo.rows * font.getCharHeight(), modeInfo.colorDepth);

    auto lfbFile = Util::File::File("/device/terminal_lfb");
    if (lfbFile.exists() && !lfbFile.remove()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Terminal: Unable to delete old lfb!");
    }

    bool success = lfbProvider.initializeLinearFrameBuffer(lfbMode, "terminal_lfb");
    if (!success || !lfbFile.exists()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Terminal: Unable to initialize lfb!");
    }

    uint8_t addressBuffer[16];
    uint8_t xBuffer[16];
    uint8_t yBuffer[16];
    uint8_t bppBuffer[16];
    uint8_t pitchBuffer[16];

    Util::Memory::Address<uint32_t>(addressBuffer, sizeof(addressBuffer)).setRange(0, sizeof(addressBuffer));
    Util::Memory::Address<uint32_t>(xBuffer, sizeof(xBuffer)).setRange(0, sizeof(xBuffer));
    Util::Memory::Address<uint32_t>(yBuffer, sizeof(yBuffer)).setRange(0, sizeof(yBuffer));
    Util::Memory::Address<uint32_t>(bppBuffer, sizeof(bppBuffer)).setRange(0, sizeof(bppBuffer));
    Util::Memory::Address<uint32_t>(pitchBuffer, sizeof(pitchBuffer)).setRange(0, sizeof(pitchBuffer));

    auto stream = Util::Stream::FileInputStream(lfbFile);
    int16_t currentChar;

    for (unsigned char &i : addressBuffer) {
        currentChar = stream.read();
        if (currentChar == '\n') {
            break;
        }

        i = currentChar;
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

    for (unsigned char & i : pitchBuffer) {
        currentChar = stream.read();
        if (currentChar == -1) {
            break;
        }

        i = currentChar;
    }

    uint32_t address = Util::Memory::String::parseInt(reinterpret_cast<const char *>(addressBuffer));
    uint16_t resolutionX = Util::Memory::String::parseInt(reinterpret_cast<const char *>(xBuffer));
    uint16_t resolutionY = Util::Memory::String::parseInt(reinterpret_cast<const char *>(yBuffer));
    uint16_t colorDepth = Util::Memory::String::parseInt(reinterpret_cast<const char *>(bppBuffer));
    uint16_t pitch = Util::Memory::String::parseInt(reinterpret_cast<const char *>(pitchBuffer));

    Terminal *terminal = new LinearFrameBufferTerminal(new Util::Graphic::LinearFrameBuffer(reinterpret_cast<void *>(address), resolutionX, resolutionY, colorDepth, pitch));

    // Create filesystem node
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>()->getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    auto *terminalNode = new TerminalNode(filename, terminal);
    return driver.addNode("/", terminalNode);
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

Util::Memory::String LinearFrameBufferTerminalProvider::getClassName() const {
    return CLASS_NAME;
}

}