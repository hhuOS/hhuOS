#include <kernel/paging/MemoryLayout.h>
#include <device/bios/Bios.h>
#include <kernel/service/FilesystemService.h>
#include <kernel/system/System.h>
#include <device/graphic/terminal/TerminalNode.h>
#include "ColorGraphicsArrayProvider.h"
#include "ColorGraphicsArray.h"

namespace Device::Graphic {

ColorGraphicsArrayProvider::ColorGraphicsArrayProvider(bool prototypeInstance) : supportedModes(2) {
    if (prototypeInstance) {
        return;
    }

    supportedModes[0] = {40, 25, 4, 0x01};
    supportedModes[1] = {80, 25, 4, 0x03};

    Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::CHECK_VIDEO_CARD;

    auto biosReturnContext = Bios::interrupt(0x10, biosParameters);
    auto cardType = static_cast<VideoCardType>(biosReturnContext.bx);

    switch (cardType) {
        case MONOCHROME:
            deviceName = "Generic MDA";
            videoMemorySize = 4096;
            break;
        case CGA_COLOR:
            deviceName = "Generic CGA";
            videoMemorySize = 16384;
            break;
        case EGA_COLOR:
        case EGA_MONOCHROME:
            deviceName = "Generic EGA";
            videoMemorySize = 131072;
            break;
        case PGA_COLOR:
            deviceName = "Generic PGA";
            videoMemorySize = 262144;
            break;
        case VGA_MONOCHROME:
        case VGA_COLOR:
            deviceName = "Generic VGA";
            videoMemorySize = 262144;
            break;
        case MCGA_COLOR_DIGITAL:
        case MCGA_MONOCHROME:
        case MCGA_COLOR:
            deviceName = "Generic MCGA";
            videoMemorySize = 65536;
            break;
        default:
            break;
    }
}

bool ColorGraphicsArrayProvider::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::CHECK_VIDEO_CARD;

    auto biosReturnContext = Bios::interrupt(0x10, biosParameters);
    auto cardType = static_cast<VideoCardType>(biosReturnContext.bx);

    return cardType > CGA_COLOR && cardType != UNKNOWN;
}

bool ColorGraphicsArrayProvider::initializeTerminal(TerminalProvider::ModeInfo &modeInfo, const Util::Memory::String &filename) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "CGA is not available on this machine!");
    }

    // Set video mode
    Bios::RealModeContext biosParameters{};
    biosParameters.ax = modeInfo.modeNumber;
    Bios::interrupt(0x10, biosParameters);

    // Set cursor options
    biosParameters.ax = BiosFunction::SET_CURSOR_SHAPE;
    biosParameters.cx = CURSOR_SHAPE_OPTIONS;
    Bios::interrupt(0x10, biosParameters);

    Device::Graphic::Terminal *terminal = new ColorGraphicsArray(modeInfo.columns, modeInfo.rows);

    // Create filesystem node
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    auto *terminalNode = new TerminalNode(filename, terminal);
    return driver.addNode("/", terminalNode);
}

Util::Data::Array<ColorGraphicsArrayProvider::ModeInfo> ColorGraphicsArrayProvider::getAvailableModes() const {
    return supportedModes;
}

uint32_t ColorGraphicsArrayProvider::getVideoMemorySize() const {
    return videoMemorySize;
}

Util::Memory::String ColorGraphicsArrayProvider::getDeviceName() const {
    return deviceName;
}

Util::Memory::String ColorGraphicsArrayProvider::getClassName() const {
    return CLASS_NAME;
}

}