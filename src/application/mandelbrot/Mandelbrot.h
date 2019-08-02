/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __Mandelbrot_include__
#define __Mandelbrot_include__

#include <kernel/thread/KernelThread.h>

#include "kernel/event/Receiver.h"
#include "kernel/log/Logger.h"
#include "xmmintrin.h"
#include "lib/graphic/ColorGradient.h"
#include "kernel/thread/Thread.h"

#include "application/ant/Limits.h"
#include "lib/math/Random.h"
#include "lib/graphic/Color.h"
#include "device/graphic/lfb/LinearFrameBuffer.h"
#include "kernel/core/System.h"

void drawMandelbrotSSE(float xlim[2], float ylim[2], uint16_t globalOffsetX, uint16_t globalOffsetY);

/**
 * Mandelbrot.
 */
class Mandelbrot : public Kernel::KernelThread, public Kernel::Receiver {

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

    void onEvent(const Kernel::Event &event) override;

    static State state;

    static Properties properties;

    static const uint32_t MAX_VALUE = 255;

    static const uint32_t MAX_COLORS = 256;

    static ColorGradient gradient;

    static LinearFrameBuffer *lfb;

private:

    Kernel::Logger &log;

    void calculatePosition();

    void drawMandelbrotNoSSE(float offsetX, float offsetY, float zoom);

    static constexpr const float X0_BASE = -2.0F;

    static constexpr const float X1_BASE = 1.0F;

    static constexpr const float Y0_BASE = -1.325F;

    static constexpr const float Y1_BASE = 1.325F;

    static constexpr const float OFFSET_STEP = 0.1F;

    static constexpr const uint8_t ZOOM_FACTOR = 12;

    bool useSSE;

    uint16_t globalOffsetX = 0;

    uint16_t globalOffsetY = 0;

    float realBase = 0.0;

    float imaginaryBase = 0.0;

    float currentOffsetX = 0.0;

    float currentOffsetY = 0.0;

    float currentZoom = 1.0;

    bool shouldDraw = true;

    bool isRunning = true;
};

#endif
