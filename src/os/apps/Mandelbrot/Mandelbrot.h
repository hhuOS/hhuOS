#ifndef __Mandelbrot_include__
#define __Mandelbrot_include__

#include <kernel/events/Receiver.h>
#include <cstdint>
#include <kernel/log/Logger.h>
#include <xmmintrin.h>
#include <lib/graphic/ColorGradient.h>
#include "kernel/threads/Thread.h"

#include "apps/AntApp/Limits.h"
#include "lib/Random.h"
#include "lib/graphic/Color.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "kernel/Kernel.h"

void drawMandelbrotSSE(float xlim[2], float ylim[2]);

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
        float xlim[2] = {-2.0F, 1.0F};
        float ylim[2] = {-1.325F, 1.325F};
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

    static const uint32_t MAX_COLORS = 256;

    static ColorGradient gradient;

    static LinearFrameBuffer *lfb;

private:

    Logger &log;

    void calculatePosition();

    void drawMandelbrotNoSSE(float offsetX, float offsetY, float zoom);

    static constexpr const float X0_BASE = -2.0F;

    static constexpr const float X1_BASE = 1.0F;

    static constexpr const float Y0_BASE = -1.325F;

    static constexpr const float Y1_BASE = 1.325F;

    static constexpr const float OFFSET_STEP = 0.1F;

    static constexpr const uint8_t ZOOM_FACTOR = 12;

    bool useSSE;

    float realBase = 0.0;

    float imaginaryBase = 0.0;

    float currentOffsetX = 0.0;

    float currentOffsetY = 0.0;

    float currentZoom = 1.0;

    bool shouldDraw = true;

    bool isRunning = true;
};

#endif
