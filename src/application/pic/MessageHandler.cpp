//
// Created by Rafael Reip on 23.10.24.
//

#include <cstring>
#include "MessageHandler.h"

MessageHandler::MessageHandler(int width, int height) {
    this->width = width;
    this->height = height;
    this->buffer = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        this->buffer[i] = 0;
    }
    this->lfb = new LinearFrameBuffer(this->buffer, width, height, 32, width * 4, false);
    this->pixelDrawer = new PixelDrawer(*lfb);
    this->stringDrawer = new StringDrawer(*pixelDrawer);
    this->messages = new Message *[20];
    this->messageCount = 0;
    this->changed = false;
    this->messageAdded = false;
    this->addMessage("hello to Pic!!! :D");

}

uint32_t *MessageHandler::getBuffer() {
    return this->buffer;
}

void MessageHandler::addMessage(const Util::String& message) {
    this->addMessage(message, 5);
}

void MessageHandler::addMessage(const Util::String& message, int duration) {
    auto *newMessage = new Message();
    newMessage->message = message;
    newMessage->expiration = time(nullptr) + duration;
    this->messages[this->messageCount] = newMessage;
    this->messageCount++;
    this->messageAdded = true;
}

bool MessageHandler::hasChangedAndReset() {
    bool temp = changed;
    changed = false;
    return temp;
}

void MessageHandler::update() {
    bool deleted = false;
    auto currentTime = time(nullptr);

    for (int i = 0; i < this->messageCount; i++) {
        if (this->messages[i]->expiration <= currentTime) {
            delete this->messages[i];
            for (int j = i; j < this->messageCount - 1; j++) {
                this->messages[j] = this->messages[j + 1];
            }
            this->messageCount--;
            i--;
            deleted = true;
        }
    }

    if (deleted || this->messageAdded) { // need to redraw
        if (deleted) {
            for (int i = 0; i < width * height; i++) {
                this->buffer[i] = 0;
            }
        }

        for (int i = 0; i < this->messageCount; i++) {
            int xStringpos = (width / 2) - (messages[i]->message.length() * 4);
            if (xStringpos < 0) xStringpos = 0;
            stringDrawer->drawString(Fonts::TERMINAL_8x16, xStringpos, i * 16, messages[i]->message.operator const char *(),
                                     Color(0, 0, 0), Color(255, 255, 255));
        }

        this->changed = true;
        this->messageAdded = false;
    }
}
