//
// Created by Rafael Reip on 23.10.24.
//

#include "MessageHandler.h"

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x16.h"

#include "helper.h"

/**
 * Constructor for the Message class.
 *
 * @param message The message content as a Util::String.
 * @param duration The duration for which the message should be displayed.
 */
Message::Message(const Util::String &message, int duration) {
    this->message = message;
    this->expiration = duration;
}

Message::~Message() = default;

/**
 * Constructor for the MessageHandler class.
 *
 * @param width The width of the message handler render buffer..
 * @param height The height of the message handler render buffer.
 */
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

/**
 * Destructor for the MessageHandler class.
 */
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

/**
 * Sets the print flag for the MessageHandler.
 *
 * @param p Boolean flag wether messages should be printed to the console.
 */
void MessageHandler::setPrintBool(bool p) {
    shouldPrint = p;
}

/**
 * Returns the render buffer of the MessageHandler.
 *
 * @return Pointer to the buffer.
 */
uint32_t *MessageHandler::getBuffer() {
    return buffer;
}

/**
 * \brief Adds a message to the MessageHandler.
 *
 * The message is added with a default duration of 5 seconds.
 *
 * @param message The message content as a Util::String.
 */
void MessageHandler::addMessage(const Util::String &message) {
    if (shouldPrint) {
        print(message);
    }
    addMessage(message, 5);
}

/**
 * Adds a message to the MessageHandler with a specified duration.
 *
 * If the message count exceeds the maximum allowed messages, the oldest message is removed.
 * If the message length exceeds the width of the buffer, it is truncated.
 *
 * @param message The message content as a Util::String.
 * @param duration The duration for which the message should be displayed.
 */
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

/**
 * Checks if the MessageHandler render buffer has changed and resets the changed flag.
 *
 * @return True if the buffer has changed, false otherwise.
 */
bool MessageHandler::hasChangedAndReset() {
    bool temp = changed;
    changed = false;
    return temp;
}

/**
 * \brief Updates the render buffer of the MessageHandler.
 *
 * This function checks for expired messages and removes them. If any messages
 * were deleted or added, it redraws the buffer.
 *
 * The function should be called once per frame.
 */
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
            int xStringpos = int((width / 2) - (messages[i]->message.length() * 4));
            if (xStringpos < 0) xStringpos = 0;
            stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, xStringpos, i * 16, messages[i]->message.operator const char *(),
                                     Util::Graphic::Color(0, 0, 0), Util::Graphic::Color(255, 255, 255));
        }

        changed = true;
        messageAdded = false;
    }
}