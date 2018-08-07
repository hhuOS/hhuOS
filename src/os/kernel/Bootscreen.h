#ifndef __Bootscreen_include__
#define __Bootscreen_include__

#include <cstdint>
#include <lib/String.h>
#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include <kernel/log/Logger.h>
#include <lib/graphic/Bmp.h>

class Bootscreen {

public:

    Bootscreen(bool showSplash, Logger &logger);

    virtual ~Bootscreen() = default;

    Bootscreen(const Bootscreen &other) = delete;

    Bootscreen &operator=(const Bootscreen &other) = delete;

    void update(uint8_t percentage, const String &message);

    void init(uint16_t xres, uint16_t yres, uint8_t bpp);

    void finish();

private:

    Logger &log;

    bool isSplashActive = false;

    LinearFrameBuffer *lfb = nullptr;

    Font *font = &std_font_8x16;

    File *logoFile = nullptr;

    Bmp *logo = nullptr;
};


#endif
