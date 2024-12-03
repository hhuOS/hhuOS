//
// Created by Rafael Reip on 02.12.24.
//

#ifndef HHUOS_SETTINGS_H
#define HHUOS_SETTINGS_H

#include "lib/libc/stdio.h"

#include "MessageHandler.h"
#include "lib/util/io/stream/BufferedInputStream.h"

class MessageHandler;

class Settings {
public:
    explicit Settings(MessageHandler *mHandler);

    ~Settings() = default;

    void resetToDefault();

    void loadFromFile();

    void saveToFile();

    bool checkeredBackground;
    bool optimizeRendering;
    bool currentLayerOverlay;
    bool activateHotkeys;
    bool showFPS;
    bool textCaptureAfterUse;
    bool resetValuesAfterConfirm;
    bool useBufferedBuffer;

private:
    MessageHandler *mHandler;
    Util::String path;
};


#endif //HHUOS_SETTINGS_H
