#include <lib/Colors.h>
#include <devices/graphics/text/fonts/Fonts.h>
#include "Bootscreen.h"
#include "bootlogo.h"
#include "Kernel.h"

extern char *gitversion;

auto versionString = String::format("hhuOS %s", gitversion);

void Bootscreen::update(uint8_t percentage, const String &message) {

    if (!isSplashActive) {

        log.info(message);

        return;
    }

    auto normalizedPercentage = static_cast<uint8_t>((percentage * 60) / 100);

    lfb->fillRect(0, 0, lfb->getResX(), lfb->getResY(), Colors::HHU_DARK_BLUE);

    lfb->placeString(sun_font_8x16, 50, 10, static_cast<char *>(versionString), Colors::HHU_GRAY, Colors::INVISIBLE);

    if(lfb->getResY() < 350) {
        lfb->placeSprite(50, 45, static_cast<uint16_t>(bootlogo_75x75.width),
                         static_cast<uint16_t>(bootlogo_75x75.height), (int32_t *) (&bootlogo_75x75.pixel_data[0]));
    } else {
        lfb->placeSprite(50, 45, static_cast<uint16_t>(bootlogo_200x200.width),
                         static_cast<uint16_t>(bootlogo_200x200.height), (int32_t *) (&bootlogo_200x200.pixel_data[0]));
    }

    lfb->placeFilledRect(20, 80, 60, 2, Colors::HHU_BLUE_30);
    lfb->placeFilledCircle(20, 81, 1, Colors::HHU_BLUE_30);
    lfb->placeFilledCircle(80, 81, 1, Colors::HHU_BLUE_30);

    lfb->placeFilledRect(20, 80, normalizedPercentage, 2, Colors::HHU_BLUE);
    lfb->placeFilledCircle(20, 81, 1, Colors::HHU_BLUE);
    lfb->placeFilledCircle(static_cast<uint16_t>(20 + normalizedPercentage), 81, 1, Colors::HHU_BLUE);

    lfb->placeString(sun_font_8x16, 50, 90, (char*) message, Colors::HHU_GRAY, Colors::INVISIBLE);

    lfb->show();
}

void Bootscreen::init(uint16_t xres, uint16_t yres, uint8_t bpp) {

    if (isSplashActive) {

        lfb->init(xres, yres, bpp);

        lfb->enableDoubleBuffering();
    }
}

void Bootscreen::finish() {

    if (isSplashActive) {

        lfb->disableDoubleBuffering();

        lfb->clear();
    }
}

Bootscreen::Bootscreen(bool showSplash, Logger &logger) : log(logger), isSplashActive(showSplash) {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
}
