/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, Jochen Peters
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
 */#include <devices/timer/Pit.h>
#include <devices/input/Keyboard.h>
#include <apps/Application.h>
#include <kernel/threads/Scheduler.h>

#include "apps/AsciimationApp/AsciimationApp.h"
#include "devices/graphics/text/fonts/Fonts.h"
#include "kernel/Kernel.h"

AsciimationApp::AsciimationApp () : Thread ("AsciimationApp") {
    eventBus = Kernel::getService<EventBus>();
    graphicsService = Kernel::getService<GraphicsService>();
    fileSystem = Kernel::getService<FileSystem>();
    timeService = Kernel::getService<TimeService>();

    memset(fileName, 0, sizeof(fileName));
}

void AsciimationApp::readLine(char *buf) {
    for(uint8_t i = 0; i < 79; i++) {
        if(*buffer == '\n') {
            buf[i] = 0;
            buffer++;
            fileLength--;
            break;
        }

        buf[i] = *buffer++;
        fileLength--;
    }
}

uint32_t AsciimationApp::readDelay() {
    char tmpBuffer[80];
    readLine(tmpBuffer);
    return static_cast<uint32_t>(strtoint(tmpBuffer));
}

void AsciimationApp::printFrame() {
    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

    int waitTime = readDelay();

    for (uint32_t i = 0; i < 13; i++) {
        char tmpBuffer[80];
        readLine(tmpBuffer);

        lfb->drawString(std_font_8x16, posX, static_cast<uint16_t>(posY + i * 16), tmpBuffer, Colors::WHITE, Colors::INVISIBLE);
    }

    lfb->show();
    timeService->msleep(static_cast<uint32_t>(waitTime * (1000 / 15)));
}

void AsciimationApp::onEvent(const Event &event) {
    TextDriver &stream = *graphicsService->getTextDriver();
    Key key = ((KeyEvent &) event).getKey();

    if(key.scancode() == KeyEvent::ESCAPE) {
        isRunning = false;
        return;
    }

    if (key.valid()) {
        if(key.ascii() == '\n') {
            fileName[strlen(fileName)] = 0;
            stream << endl;

            File *file = File::open(fileName, "r");
            if(file != nullptr) {
                this->file = file;
                return;
            }

            stream << endl << "File not found!" << endl << ">";
            stream.flush();
        } else if(key.ascii() == '\b') {
            if(strlen(fileName) > 0) {
                uint16_t x, y;
                stream.getpos(x, y);
                stream.show(x, y, ' ', Colors::BLACK, Colors::BLACK);
                stream.show(--x, y, ' ', Colors::BLACK, Colors::BLACK);
                stream.setpos(x, y);

                memset(&fileName[strlen(fileName) - 1], 0, sizeof(fileName) - (strlen(fileName) - 1));
            }
        } else {
            fileName[strlen(fileName)] = key.ascii();
            stream << key.ascii();
            stream.flush();
        }
    }
}

void AsciimationApp::run () {
    TextDriver &stream = *graphicsService->getTextDriver();
    stream << "Please enter the path to a valid Asciimation-file:" << endl << ">";
    stream.flush();

    eventBus->subscribe(*this, KeyEvent::TYPE);

    while(file == nullptr && isRunning) {}

    if(isRunning) {
        LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
        lfb->init(640, 400, 16);

        posX = static_cast<uint16_t>((lfb->getResX() / 2) - ((68 * 8) / 2));
        posY = static_cast<uint16_t>((lfb->getResY() / 2) - ((13 * 16) / 2));

        lfb->placeString(std_font_8x16, 50, 50, "Reading file...", Colors::WHITE, Colors::INVISIBLE);

        *file >> buffer;

        fileLength = file->getLength();

        lfb->enableDoubleBuffering();
        lfb->clear();

        while (fileLength > 0 && isRunning) {
            printFrame();
        }
    }

    eventBus->unsubscribe(*this, KeyEvent::TYPE);
    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}
