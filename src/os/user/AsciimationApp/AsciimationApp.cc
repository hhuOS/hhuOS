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
#include <devices/input/Keyboard.h>

#include "user/AsciimationApp/AsciimationApp.h"
#include "devices/graphics/text/fonts/Fonts.h"
#include "kernel/Kernel.h"

extern "C" {
    #include "lib/libc/string.h"
}

AsciimationApp::AsciimationApp () : Thread ("AsciimationApp") {
    eventBus = (EventBus*) Kernel::getService(EventBus::SERVICE_NAME);
    graphicsService = (GraphicsService*) Kernel::getService(GraphicsService::SERVICE_NAME);
    fileSystem = (FileSystem*) Kernel::getService(FileSystem::SERVICE_NAME);
    timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);

    memset(fileName, 0, 4096);
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


    if(fileLength <= 0) {
        lfb->placeString(std_font_8x16, 50, 50, "End of file!", Colors::WHITE, Colors::INVISIBLE);
        lfb->show();
        return;
    }

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
                fileName[strlen(fileName) - 1] = 0;
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

    while(file == nullptr) {}

    Pit::getInstance()->setCursor(false);

    LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();
    lfb->init(800, 600, 32);

    posX = static_cast<uint16_t>((lfb->getResX() / 2) - ((68 * 8) / 2));
    posY = static_cast<uint16_t>((lfb->getResY() / 2) - ((13 * 16) / 2));

    lfb->placeString(std_font_8x16, 50, 50, "Reading file...", Colors::WHITE, Colors::INVISIBLE);

    DirEntry *info = file->getInfo();
    buffer = new char[info->length];
    *file >> buffer;

    fileLength = info->length;

    lfb->enableDoubleBuffering();
    lfb->clear();

    while(true) {
        printFrame();
    }
}
