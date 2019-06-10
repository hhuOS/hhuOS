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

#include "lib/file/File.h"
#include "kernel/event/input/KeyEvent.h"
#include "kernel/service/TimeService.h"
#include "BmpView.h"

BmpView::BmpView(Shell &shell) : Command(shell) {

}

void BmpView::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    String absolutePath = calcAbsolutePath(parser.getUnnamedArguments()[0]);

    bmp = Bmp::load(absolutePath);

    if(bmp == nullptr) {
        stderr << args[0] << ": Unable to open file '" << parser.getUnnamedArguments()[0] << "'!" << endl;
        return;
    }

    LinearFrameBuffer *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    double horizontalScaling = 1.0 / (lfb->getResX() / (bmp->getWidth() + 30.0));
    double verticalScaling = 1.0 / (lfb->getResY() / (bmp->getHeight() + 65.0));

    scalingFactor = 1;
    scalingDividor = static_cast<uint8_t>(horizontalScaling > verticalScaling ? horizontalScaling + 1 : verticalScaling + 1);

    if(scalingDividor < 1) {
        scalingDividor = 1;
    }

    initScalingDividor = scalingDividor;

    lfb->init(lfb->getResX(), lfb->getResY(), 32);
    lfb->enableDoubleBuffering();

    drawBitmap();

    isRunning = true;

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    while(isRunning);

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    delete bmp;

    TextDriver *text = Kernel::getService<GraphicsService>()->getTextDriver();
    text->init(text->getColumnCount(), text->getRowCount(), text->getDepth());
}

const String BmpView::getHelpText() {
    return "Shows a BMP-file.\n\n"
           "Usage: bmpview [OPTION]... [PATH]\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}

void BmpView::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent&) event;

    if(keyEvent.getKey().scancode() == KeyEvent::RETURN) {
        isRunning = false;
    } else if(keyEvent.getKey().ascii() == '+') {
        scaleUp();
    } else if(keyEvent.getKey().ascii() == '-') {
        scaleDown();
    }
}

void BmpView::scaleUp() {
    LinearFrameBuffer *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    if(scalingDividor > initScalingDividor) {
        scalingDividor--;
        drawBitmap();
    } else if(scalingFactor >= 1) {
        if((bmp->getWidth() * scalingFactor * 2) < static_cast<uint32_t>(lfb->getResX() + 30) &&
                (bmp->getHeight() * scalingFactor * 2) < static_cast<uint32_t>(lfb->getResY() + 50)) {
            scalingFactor++;
            drawBitmap();
        }
    }
}

void BmpView::scaleDown() {
    if(scalingFactor > 1) {
        scalingFactor--;
        drawBitmap();
    } else if(scalingDividor >= initScalingDividor) {
        if((bmp->getWidth() / scalingDividor / 2) >= 36 && (bmp->getHeight() / scalingDividor / 2) >= 4) {
            scalingDividor++;
            drawBitmap();
        }
    }
}

void BmpView::drawBitmap() {
    LinearFrameBuffer *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
    String returnString = "<RETURN>";

    Bmp scaledBmp(*bmp);

    if(scalingFactor > 1) {
        scaledBmp.scaleUp(scalingFactor);
    } else if(scalingDividor > 1) {
        scaledBmp.scaleDown(scalingDividor);
    }

    lfb->clear();
    
    lfb->fillRect(static_cast<uint16_t>((lfb->getResX() - scaledBmp.getWidth()) / 2 - 15),
                  static_cast<uint16_t>((lfb->getResY() - scaledBmp.getHeight()) / 2 - 15),
                  static_cast<uint16_t>(scaledBmp.getWidth() + 30),
                  static_cast<uint16_t>(scaledBmp.getHeight() + 50), Colors::HHU_LIGHT_GRAY);

    lfb->drawRect(static_cast<uint16_t>((lfb->getResX() - scaledBmp.getWidth()) / 2 - 1),
                  static_cast<uint16_t>((lfb->getResY() - scaledBmp.getHeight()) / 2 - 1),
                  static_cast<uint16_t>(scaledBmp.getWidth() + 1),
                  static_cast<uint16_t>(scaledBmp.getHeight() + 1), Colors::GRAY);

    lfb->drawString(std_font_8x16,
            static_cast<uint16_t>(lfb->getResX() / 2 - ( std_font_8x16.get_char_width() * returnString.length()) / 2),
            static_cast<uint16_t>((lfb->getResY() + scaledBmp.getHeight()) / 2 + (45 - std_font_8x16.get_char_height()) / 2),
            "<RETURN>", Colors::BLACK, Colors::INVISIBLE);

    scaledBmp.draw(static_cast<uint16_t>((lfb->getResX() - scaledBmp.getWidth()) / 2),
            static_cast<uint16_t>((lfb->getResY() - scaledBmp.getHeight()) / 2));

    lfb->show();
}