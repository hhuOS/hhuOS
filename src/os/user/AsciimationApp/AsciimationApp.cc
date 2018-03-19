/*****************************************************************************
 *                                                                           *
 *                          S T A R W A R S                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Star Wars Episode 1.                                     *
 *                                                                           *
 * Autor:           Filip Krakowski, 20.10.2017                              *
 *****************************************************************************/

#include <devices/Pit.h>

#include "user/AsciimationApp/AsciimationApp.h"
#include "devices/graphics/text/fonts/Fonts.h"
#include "kernel/Kernel.h"

extern "C" {
    #include "lib/libc/string.h"
}

AsciimationApp::AsciimationApp (const char *path) : Thread ("AsciimationApp") {
    filePath = path;
    timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);
}

void AsciimationApp::readLine(char *buf) {
    for(uint8_t i = 0; i < 79; i++) {
        if(*buffer == '\n') {
            buf[i] = 0;
            buffer++;
            break;
        }

        buf[i] = *buffer++;
    }
}

uint32_t AsciimationApp::readDelay() {
    char tmpBuffer[80];
    readLine(tmpBuffer);
    return strtoint(tmpBuffer);
}

void AsciimationApp::printFrame() {
    int waitTime = readDelay();

    for (uint32_t i = 0; i < 13; i++) {
        char tmpBuffer[80];
        readLine(tmpBuffer);

        lfb->drawString(std_font_8x16, posX, posY + i * 16, tmpBuffer, Colors::WHITE, Colors::INVISIBLE);
    }

    lfb->show();
    timeService->msleep(waitTime * (1000/15));
}

void AsciimationApp::run () {
    lfb = ((GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME))->getLinearFrameBuffer();

    posX = (lfb->getResX() / 2) - ((68 * 8) / 2);
    posY = (lfb->getResY() / 2) - ((13 * 16) / 2);
    Pit::getInstance()->setCursor(false);

    lfb->clear();

    file = File::open(filePath, "r");

    if(file == nullptr) {
        lfb->placeString(std_font_8x16, 50, 50, "File not found!", Colors::WHITE, Colors::INVISIBLE);
        while(1);
    }

    lfb->placeString(std_font_8x16, 50, 50, "Reading file...", Colors::WHITE, Colors::INVISIBLE);

    DirEntry *info = file->getInfo();
    buffer = new char[info->length];
    *file >> buffer;

    lfb->enableDoubleBuffering();
    lfb->clear();

    while(1) {
        printFrame();
    }
}
