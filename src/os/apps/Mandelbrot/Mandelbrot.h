#ifndef __Mandelbrot_include__
#define __Mandelbrot_include__

#include <kernel/events/Receiver.h>
#include <cstdint>
#include <kernel/log/Logger.h>
#include <xmmintrin.h>
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

    struct State {
        __m128 xmin;
        __m128 ymin;
        __m128 xscale;
        __m128 yscale;
        __m128 threshold;
        __m128 one;
        __m128 iter_scale;
        __m128 depth_scale;
        __m128 mx;
        __m128 my;
        __m128 cr;
        __m128 ci;
        __m128 zr;
        __m128 zi;
        __m128 mk;
        __m128 zr2;
        __m128 zi2;
        __m128 zrzi;
        __m128 mag2;
        __m128 mask;
        __m128i pixels;
    } __attribute__ ((aligned (16)));

    struct Properties {
        uint32_t width = 640;
        uint32_t height = 480;
        uint32_t depth = 256;
        uint32_t iterations = 256;
        float xlim[2] = {-2.1, 0.6F};
        float ylim[2] = {-1.35F, 1.35F};
    } __attribute__ ((aligned (16)));

public:

    Mandelbrot();

    Mandelbrot(const Mandelbrot &other) = delete;

    Mandelbrot &operator=(const Mandelbrot &other) = delete;

    ~Mandelbrot() override = default;

    void run() override;

    void onEvent(const Event &event) override;

    static State state;

    static Properties properties;

    static const uint32_t MAX_VALUE = 255;

    static const uint32_t MAX_COLORS = 5;

    static Color colors[MAX_COLORS];

    static LinearFrameBuffer *lfb;

private:

    Logger &log;

    Color colorAt(uint32_t x, uint32_t y, double offsetX, double offsetY, double zoom);

    double realBase = 0.0;

    double imaginaryBase = 0.0;

    double currentOffsetX = -0.74560698146851179;

    double currentOffsetY = -0.12078252355012835;

    double currentZoom = 74.061564435308497;

    bool shouldDraw = true;

    bool isRunning = true;
};

#endif
