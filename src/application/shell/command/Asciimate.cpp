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

#include "kernel/event/input/KeyEvent.h"
#include "lib/file/FileStatus.h"
#include "Asciimate.h"

Asciimate::Asciimate(Shell &shell) : Command(shell) {

}

void Asciimate::execute(Util::Array<String> &args) {
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

    FileStatus *fStat = FileStatus::stat(absolutePath);

    File *file = File::open(absolutePath, "r");

    if(file == nullptr) {
        stderr << args[0] << ": Unable to open file '" << parser.getUnnamedArguments()[0] << "'!" << endl;
        return;
    }

    timeService = Kernel::System::getService<Kernel::TimeService>();
    lfb = Kernel::System::getService<Kernel::GraphicsService>()->getLinearFrameBuffer();

    stdout << "Playing '" << fStat->getName() << "'." << endl;
    stdout << "Press <ESC> to stop." << endl << endl;

    Kernel::System::getService<Kernel::EventBus>()->subscribe(*this, Kernel::KeyEvent::TYPE);

    play(file);

    Kernel::System::getService<Kernel::EventBus>()->unsubscribe(*this, Kernel::KeyEvent::TYPE);

    TextDriver *text = Kernel::System::getService<Kernel::GraphicsService>()->getTextDriver();
    text->init(text->getColumnCount(), text->getRowCount(), text->getDepth());

    delete fStat;
    delete file;
}

void Asciimate::play(File *file) {
    stdout << "Reading file...";
    stdout.flush();

    fileLength = file->getLength();
    *file >> asciimationBuffer;

    stdout << " Finished!" << endl << endl;

    stdout << "Starting in 3...";
    stdout.flush();
    timeService->msleep(1000);

    stdout << "2...";
    stdout.flush();
    timeService->msleep(1000);

    stdout << "1...";
    stdout.flush();
    timeService->msleep(1000);

    lfb->init(640, 400, 16);
    lfb->enableDoubleBuffering();
    lfb->clear();
    lfb->show();

    posX = static_cast<uint16_t>((lfb->getResX() / 2) - ((FRAME_WIDTH * font.get_char_width()) / 2));
    posY = static_cast<uint16_t>((lfb->getResY() / 2) - ((FRAME_HEIGHT * font.get_char_height()) / 2));

    rectWidth = FRAME_WIDTH * font.get_char_width();
    rectHeight = FRAME_HEIGHT * font.get_char_height();

    isRunning = true;

    while (fileLength > 0 && isRunning) {
        drawFrame();
    }
}

void Asciimate::drawFrame() {
    int waitTime = readDelay();

    for (uint32_t i = 0; i < 13; i++) {
        char tmpBuffer[80];
        readLine(tmpBuffer);

        lfb->drawString(font, posX, static_cast<uint16_t>(posY + i * font.get_char_height()), tmpBuffer,
                Colors::WHITE, Colors::INVISIBLE);
    }

    lfb->drawRect(posX, posY, rectWidth, rectHeight, Colors::WHITE);

    lfb->show();
    timeService->msleep(static_cast<uint32_t>(waitTime * (1000 / 15)));
}

void Asciimate::readLine(char *buf) {
    for(uint8_t i = 0; i < 79; i++) {
        if(*asciimationBuffer == '\n') {
            buf[i] = 0;
            asciimationBuffer++;
            fileLength--;
            break;
        }

        buf[i] = *asciimationBuffer++;
        fileLength--;
    }
}

uint32_t Asciimate::readDelay() {
    char tmpBuffer[80];

    readLine(tmpBuffer);

    return static_cast<uint32_t>(strtoint(tmpBuffer));
}

const String Asciimate::getHelpText() {
    return "Plays an Asciimation-file.\n\n"
           "Usage: asciimate [OPTION]... [PATH]\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}

void Asciimate::onEvent(const Kernel::Event &event) {
    auto &keyEvent = (Kernel::KeyEvent&) event;

    if(keyEvent.getKey().scancode() == Kernel::KeyEvent::ESCAPE) {
        isRunning = false;
    }
}
