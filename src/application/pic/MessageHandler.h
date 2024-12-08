//
// Created by Rafael Reip on 23.10.24.
//

#ifndef HHUOS_MESSAGEHANDLER_H
#define HHUOS_MESSAGEHANDLER_H

#include <cstdint>
#include "lib/libc/time.h"
#include "lib/util/base/String.h"

namespace Util::Graphic {
    class LinearFrameBuffer;

    class PixelDrawer;

    class StringDrawer;
}

class Message {
public:
    Message(const Util::String &message, int duration);
    ~Message();
    Util::String message;
    time_t expiration;
};

class MessageHandler {
public:

    MessageHandler(int width, int height);

    ~MessageHandler();

    void update();

    bool hasChangedAndReset();

    uint32_t *getBuffer();

    void addMessage(const Util::String &message);

    void addMessage(const Util::String &message, int duration);

    void setPrintBool(bool print);

private:
    uint32_t width, height;
    uint32_t *buffer;
    Util::Graphic::LinearFrameBuffer *lfb;
    Util::Graphic::PixelDrawer *pixelDrawer;
    Util::Graphic::StringDrawer *stringDrawer;

    bool changed;
    bool overflowed;
    bool messageAdded;

    Message **messages;
    int messageCount;
    int maxMessages;

    bool shouldPrint;
};


#endif //HHUOS_MESSAGEHANDLER_H
