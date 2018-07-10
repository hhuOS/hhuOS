#ifndef __Mandelbrot_include__
#define __Mandelbrot_include__

#include <kernel/events/Receiver.h>
#include <cstdint>
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

    Color colorAt(uint32_t x, uint32_t y, double offsetX, double offsetY, double zoom);

    LinearFrameBuffer *lfb = nullptr;

    static const uint32_t MAX_VALUE = 255;

    double currentOffsetX = 0.0;

    double currentOffsetY = 0.0;

    double currentZoom = 1.0;

    bool shouldDraw = true;

    bool isRunning = true;
};

#endif
