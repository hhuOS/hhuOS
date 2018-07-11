#ifndef __Mandelbrot_include__
#define __Mandelbrot_include__

#include <kernel/events/Receiver.h>
#include <cstdint>
#include <kernel/log/Logger.h>
#include "kernel/threads/Thread.h"

#include "apps/AntApp/Limits.h"
#include "lib/Random.h"
#include "lib/graphic/Color.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "kernel/Kernel.h"

/**
 * Mandelbrot.
 */
class Mandelbrot : public Thread, public Receiver {

public:

    Mandelbrot();

    Mandelbrot(const Mandelbrot &other) = delete;

    Mandelbrot &operator=(const Mandelbrot &other) = delete;

    ~Mandelbrot() override = default;

    void run() override;

    void onEvent(const Event &event) override;

private:

    Logger &log;

    Color colorAt(uint32_t x, uint32_t y, double offsetX, double offsetY, double zoom);

    LinearFrameBuffer *lfb = nullptr;

    static constexpr const double START_REAL = 0.0;

    static constexpr const double START_IMAGINARY = 0.0;

    static const uint32_t MAX_VALUE = 255;

    static const uint32_t MAX_COLORS = 5;

    static Color colors[MAX_COLORS];

    double realBase = 0.0;

    double imaginaryBase = 0.0;

    double currentOffsetX = -0.74560698146851179;

    double currentOffsetY = -0.12078252355012835;

    double currentZoom = 74.061564435308497;

    bool shouldDraw = true;

    bool isRunning = true;

    void mandel();
};

#endif
