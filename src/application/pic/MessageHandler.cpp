//
// Created by Rafael Reip on 23.10.24.
//

#include "MessageHandler.h"

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x16.h"

#include "helper.h"

Message::Message(const Util::String &message, int duration) {
    this->message = message;
    this->expiration = duration;
}

Message::~Message() = default;

MessageHandler::MessageHandler(int width, int height) {
    this->width = width;
    this->height = height;
    this->buffer = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        this->buffer[i] = 0;
    }
    this->lfb = new Util::Graphic::LinearFrameBuffer(buffer, width, height, 32, width * 4, false);
    this->pixelDrawer = new Util::Graphic::PixelDrawer(*lfb);
    this->stringDrawer = new Util::Graphic::StringDrawer(*pixelDrawer);
    this->maxMessages = height / 16;
    this->messages = new Message *[maxMessages];
    this->messageCount = 0;
    this->changed = false;
    this->messageAdded = false;
    this->overflowed = false;
    this->shouldPrint = false;
    addMessage("hello to Pic!!! :D");
}

MessageHandler::~MessageHandler() {
    for (int i = 0; i < messageCount; i++) {
        delete messages[i];
    }
    delete[] messages;
    delete pixelDrawer;
    delete stringDrawer;
    delete lfb;
    delete[] buffer;
}

void MessageHandler::setPrintBool(bool p) {
    shouldPrint = p;
}

uint32_t *MessageHandler::getBuffer() {
    return buffer;
}

void MessageHandler::addMessage(const Util::String &message) {
    if (shouldPrint) {
        print(message);
    }
    addMessage(message, 5);
}

void MessageHandler::addMessage(const Util::String &message, int duration) {
    if (messageCount >= maxMessages) {
        delete messages[0];
        for (int i = 0; i < messageCount - 1; i++) {
            messages[i] = messages[i + 1];
        }
        messageCount--;
        overflowed = true;
    }

    if (message.length() * 8 > width) {
        uint32_t maxChars = width / 8;
        Util::String newMessage = message.substring(0, maxChars);
        addMessage(newMessage, duration);
        return;
    }

    auto *newMessage = new Message(message, time(nullptr) + duration);
    messages[messageCount] = newMessage;
    messageCount++;
    messageAdded = true;
}

bool MessageHandler::hasChangedAndReset() {
    bool temp = changed;
    changed = false;
    return temp;
}

void MessageHandler::update() {
    bool deleted = false;
    auto currentTime = time(nullptr);

    for (int i = 0; i < messageCount; i++) {
        if (messages[i]->expiration <= currentTime) {
            delete messages[i];
            for (int j = i; j < messageCount - 1; j++) {
                messages[j] = messages[j + 1];
            }
            messageCount--;
            i--;
            deleted = true;
        }
    }

    if (deleted || messageAdded) { // need to redraw
        if (deleted || overflowed) {
            for (uint32_t i = 0; i < width * height; i++) {
                buffer[i] = 0;
            }
        }

        for (int i = 0; i < messageCount; i++) {
            int xStringpos = (width / 2) - (messages[i]->message.length() * 4);
            if (xStringpos < 0) xStringpos = 0;
            stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, xStringpos, i * 16, messages[i]->message.operator const char *(),
                                     Util::Graphic::Color(0, 0, 0), Util::Graphic::Color(255, 255, 255));
        }

        changed = true;
        messageAdded = false;
    }
}