//
// Created by Rafael Reip on 23.10.24.
//

#ifndef HHUOS_MESSAGEHANDLER_H
#define HHUOS_MESSAGEHANDLER_H

#include "lib/libc/time.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x16.h"
#include "DataWrapper.h"

using namespace Util::Graphic;

class Message {
public:
    Util::String message;
    time_t expiration;
};

class MessageHandler {
public:

    MessageHandler(int width, int height);

    void update();

    bool hasChangedAndReset();

    uint32_t *getBuffer();

    void addMessage(const Util::String& message);

    void addMessage(const Util::String& message, int duration);

private:
    int width, height;
    uint32_t *buffer;
    LinearFrameBuffer *lfb;
    PixelDrawer *pixelDrawer;
    StringDrawer *stringDrawer;

    bool changed;
    bool messageAdded;

    Message **messages;
    int messageCount;


};


#endif //HHUOS_MESSAGEHANDLER_H
