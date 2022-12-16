#include "device/bios/Bios.h"
#include "ColorGraphicsAdapterProvider.h"
#include "kernel/system/BlueScreen.h"
#include "ColorGraphicsAdapter.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"
#include "lib/util/graphic/Terminal.h"
#include "lib/util/memory/Address.h"

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
    log.info("CGA compatible card found (type: [%s])", static_cast<const char*>(getVideoCardTypeAsString(cardType)));
}

bool ColorGraphicsAdapterProvider::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::CHECK_VIDEO_CARD;

    auto biosReturn = Bios::interrupt(0x10, biosParameters);
    auto cardType = static_cast<VideoCardType>(biosReturn.bx);

    return cardType > CGA_COLOR && cardType != UNKNOWN;
}

Util::Graphic::Terminal* ColorGraphicsAdapterProvider::initializeTerminal(const ModeInfo &modeInfo) {
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

    return terminal;
}

Util::Data::Array<ColorGraphicsAdapterProvider::ModeInfo> ColorGraphicsAdapterProvider::getAvailableModes() const {
    return supportedModes;
}

ColorGraphicsAdapterProvider::VideoCardType ColorGraphicsAdapterProvider::getVideoCardType() {
    Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::CHECK_VIDEO_CARD;

    auto biosReturn = Bios::interrupt(0x10, biosParameters);
    return static_cast<VideoCardType>(biosReturn.bx);
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