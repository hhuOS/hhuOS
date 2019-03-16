/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <lib/graphic/Colors.h>
#include <devices/graphics/text/fonts/Fonts.h>
#include <BuildConfig.h>
#include <lib/file/bmp/Bmp.h>
#include <kernel/services/GraphicsService.h>
#include "Bootscreen.h"
#include "Kernel.h"

auto versionString = String::format("hhuOS %s - git %s (%s)", BuildConfig::VERSION, BuildConfig::GIT_REV, BuildConfig::GIT_BRANCH);
auto buildDate = String::format("Build date: %s", BuildConfig::BUILD_DATE);

void Bootscreen::update(uint8_t percentage, const String &message) {

    if (!isSplashActive) {

        log.trace(message);

        return;
    }

    auto normalizedPercentage = static_cast<uint8_t>((percentage * 60) / 100);

    lfb->fillRect(0, 0, lfb->getResX(), lfb->getResY(), Colors::HHU_DARK_BLUE);

    lfb->placeString(*font, 50, 10, static_cast<char *>(versionString), Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->placeString(*font, 50, 15, static_cast<char *>(buildDate), Colors::HHU_GRAY, Colors::INVISIBLE);

    if(logo != nullptr) {
        logo->draw(static_cast<uint16_t>((lfb->getResX() - logo->getWidth()) / 2),
                   static_cast<uint16_t>((lfb->getResY() - logo->getHeight()) / 2));
    }

    lfb->placeFilledRect(20, 85, 60, 2, Colors::HHU_BLUE_10);
    lfb->placeFilledCircle(20, 86, 1, Colors::HHU_BLUE_10);
    lfb->placeFilledCircle(80, 86, 1, Colors::HHU_BLUE_10);

    lfb->placeFilledRect(20, 85, normalizedPercentage, 2, Colors::HHU_BLUE);
    lfb->placeFilledCircle(20, 86, 1, Colors::HHU_BLUE);
    lfb->placeFilledCircle(static_cast<uint16_t>(20 + normalizedPercentage), 86, 1, Colors::HHU_BLUE);

    lfb->placeString(*font, 50, 90, (char*) message, Colors::HHU_GRAY, Colors::INVISIBLE);

    lfb->show();
}

void Bootscreen::init(uint16_t xres, uint16_t yres, uint8_t bpp) {

    if (isSplashActive) {

        lfb->init(xres, yres, bpp);

        // Don't use High-Res mode on CGA, as it looks bad.
        if(lfb->getDepth() == 1) {
            lfb->init(320, 200, 2);
        }

        lfb->enableDoubleBuffering();

        logo = Bmp::load("/os/boot-logo.bmp");

        if (logo != nullptr) {
            double scaling = (static_cast<double>(lfb->getResY()) / 600);

            if (scaling >= 1) {
                if(scaling - static_cast<uint8_t>(scaling) >= 0.8) {
                    scaling++;
                }

                logo->scaleUp(static_cast<uint8_t>(scaling));
            } else {
                scaling = 1 / scaling;

                if(scaling - static_cast<uint8_t>(scaling) >= 0.2) {
                    scaling++;
                }

                logo->scaleDown(static_cast<uint8_t>(scaling));
            }
        }
    }
}

void Bootscreen::finish() {

    if (isSplashActive) {

        lfb->disableDoubleBuffering();

        lfb->clear();

        delete logo;
    }
}

Bootscreen::Bootscreen(bool showSplash, Logger &logger) : log(logger), isSplashActive(showSplash) {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
}
