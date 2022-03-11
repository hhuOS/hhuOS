#include "kernel/paging/MemoryLayout.h"
#include "device/bios/Bios.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "device/graphic/terminal/TerminalNode.h"
#include "ColorGraphicsAdapterProvider.h"
#include "ColorGraphicsAdapter.h"
#include "kernel/system/BlueScreen.h"

namespace Device::Graphic {

Kernel::Logger ColorGraphicsAdapterProvider::log = Kernel::Logger::get("CGA");

ColorGraphicsAdapterProvider::ColorGraphicsAdapterProvider(bool prototypeInstance) {
    if (prototypeInstance) {
        return;
    }

    log.info("Retrieving video card info");
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ColorGraphicsAdapterProvider: No CGA compatible card found!");
    }

    auto cardType = getVideoCardType();
    log.info("CGA compatible card found (Type: [%s])", static_cast<const char*>(getVideoCardTypeAsString(cardType)));
}

bool ColorGraphicsAdapterProvider::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::CHECK_VIDEO_CARD;

    auto biosReturnContext = Bios::interrupt(0x10, biosParameters);
    auto cardType = static_cast<VideoCardType>(biosReturnContext.bx);

    return cardType > CGA_COLOR && cardType != UNKNOWN;
}

void ColorGraphicsAdapterProvider::initializeTerminal(TerminalProvider::ModeInfo &modeInfo, const Util::Memory::String &filename) {
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

    auto *terminal = new ColorGraphicsAdapter(modeInfo.columns, modeInfo.rows);
    Kernel::BlueScreen::setCgaMode(terminal->getAddress().get(), terminal->getColumns(), terminal->getRows());

    // Create filesystem node
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    auto *terminalNode = new TerminalNode(filename, terminal);

    if (!driver.addNode("/", terminalNode)) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CGA: Unable to add node!");
    }
}

Util::Data::Array<ColorGraphicsAdapterProvider::ModeInfo> ColorGraphicsAdapterProvider::getAvailableModes() const {
    return supportedModes;
}

Util::Memory::String ColorGraphicsAdapterProvider::getClassName() const {
    return CLASS_NAME;
}

ColorGraphicsAdapterProvider::VideoCardType ColorGraphicsAdapterProvider::getVideoCardType() {
    Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::CHECK_VIDEO_CARD;

    auto biosReturnContext = Bios::interrupt(0x10, biosParameters);
    return static_cast<VideoCardType>(biosReturnContext.bx);
}

Util::Memory::String ColorGraphicsAdapterProvider::getVideoCardTypeAsString(VideoCardType cardType) {
    switch (cardType) {
        case MONOCHROME:
            return "Monochrome Display Adapter";
        case CGA_COLOR:
            return "Color Graphics Adapter";
        case EGA_COLOR:
        case EGA_MONOCHROME:
            return "Enhanced Graphics Adapter";
        case PGA_COLOR:
            return "Professional Graphics Controller";
        case VGA_MONOCHROME:
        case VGA_COLOR:
            return "Video Graphics Array";
        case MCGA_COLOR_DIGITAL:
        case MCGA_MONOCHROME:
        case MCGA_COLOR:
            return "Multi-Color Graphics Array";
        default:
            return UNKNOWN;
    }
}

}