#include "Mandelbrot.h"
#include <kernel/services/GraphicsService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/EventBus.h>
#include <apps/Application.h>
#include <lib/math/Complex.h>

#include <xmmintrin.h>

Color Mandelbrot::colors[MAX_COLORS] {
        Color(0, 0, 0),
        Color(255, 85, 84),
        Color(255, 188, 41),
        Color(59, 178, 115),
        Color(119, 104, 174)
};

__attribute__ ((aligned (16))) Mandelbrot::Properties Mandelbrot::properties;

__attribute__ ((aligned (16))) Mandelbrot::State Mandelbrot::state;

LinearFrameBuffer *Mandelbrot::lfb = nullptr;

Mandelbrot::Mandelbrot() : Thread("Mandelbrot"), log(Logger::get("Mandelbrot")) {

}

__attribute__((force_align_arg_pointer)) void drawMandelbrot() {

    Mandelbrot::state.xmin = _mm_set_ps1(Mandelbrot::properties.xlim[0]);
    Mandelbrot::state.ymin = _mm_set_ps1(Mandelbrot::properties.ylim[0]);
    Mandelbrot::state.xscale = _mm_set_ps1((Mandelbrot::properties.xlim[1] - Mandelbrot::properties.xlim[0]) / Mandelbrot::properties.width);
    Mandelbrot::state.yscale = _mm_set_ps1((Mandelbrot::properties.ylim[1] - Mandelbrot::properties.ylim[0]) / Mandelbrot::properties.height);
    Mandelbrot::state.threshold = _mm_set_ps1(4);
    Mandelbrot::state.one = _mm_set_ps1(1);
    Mandelbrot::state.iter_scale = _mm_set_ps1(1.0F / Mandelbrot::properties.iterations);
    Mandelbrot::state.depth_scale = _mm_set_ps1(Mandelbrot::properties.depth - 1);

    for (uint32_t y = 0; y < Mandelbrot::properties.height; y++) {

        for (uint32_t x = 0; x < Mandelbrot::properties.width; x += 4) {

            Mandelbrot::state.mx = _mm_set_ps(x + 3, x + 2, x + 1, x + 0);
            Mandelbrot::state.my = _mm_set_ps1(y);
            Mandelbrot::state.cr = _mm_add_ps(_mm_mul_ps(Mandelbrot::state.mx, Mandelbrot::state.xscale), Mandelbrot::state.xmin);
            Mandelbrot::state.ci = _mm_add_ps(_mm_mul_ps(Mandelbrot::state.my, Mandelbrot::state.yscale), Mandelbrot::state.ymin);
            Mandelbrot::state.zr = Mandelbrot::state.cr;
            Mandelbrot::state.zi = Mandelbrot::state.ci;

            uint32_t k = 1;
            Mandelbrot::state.mk = _mm_set_ps1(k);

            while (++k < Mandelbrot::properties.iterations) {

                Mandelbrot::state.zr2 = _mm_mul_ps(Mandelbrot::state.zr, Mandelbrot::state.zr);
                Mandelbrot::state.zi2 = _mm_mul_ps(Mandelbrot::state.zi, Mandelbrot::state.zi);
                Mandelbrot::state.zrzi = _mm_mul_ps(Mandelbrot::state.zr, Mandelbrot::state.zi);
                Mandelbrot::state.zr = _mm_add_ps(_mm_sub_ps(Mandelbrot::state.zr2, Mandelbrot::state.zi2), Mandelbrot::state.cr);
                Mandelbrot::state.zi = _mm_add_ps(_mm_add_ps(Mandelbrot::state.zrzi, Mandelbrot::state.zrzi), Mandelbrot::state.ci);

                Mandelbrot::state.zr2 = _mm_mul_ps(Mandelbrot::state.zr, Mandelbrot::state.zr);
                Mandelbrot::state.zi2 = _mm_mul_ps(Mandelbrot::state.zi, Mandelbrot::state.zi);
                Mandelbrot::state.mag2 = _mm_add_ps(Mandelbrot::state.zr2, Mandelbrot::state.zi2);
                Mandelbrot::state.mask = _mm_cmplt_ps(Mandelbrot::state.mag2, Mandelbrot::state.threshold);
                Mandelbrot::state.mk = _mm_add_ps(_mm_and_ps(Mandelbrot::state.mask, Mandelbrot::state.one), Mandelbrot::state.mk);

                if (_mm_movemask_ps(Mandelbrot::state.mask) == 0) {

                    break;
                }
            }

            Mandelbrot::state.mk = _mm_mul_ps(Mandelbrot::state.mk, Mandelbrot::state.iter_scale);
            Mandelbrot::state.mk = _mm_sqrt_ps(Mandelbrot::state.mk);
            Mandelbrot::state.mk = _mm_mul_ps(Mandelbrot::state.mk, Mandelbrot::state.depth_scale);
            Mandelbrot::state.pixels = _mm_cvtps_epi32(Mandelbrot::state.mk);

            uint32_t *src = (uint32_t*) &Mandelbrot::state.pixels;

            for (int i = 0; i < 4; i++) {

                uint32_t base = src[i * 4];

                Mandelbrot::lfb->drawPixel(x + i, y, Mandelbrot::colors[base & Mandelbrot::MAX_COLORS]);
            }
        }
    }
}



void Mandelbrot::run() {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    lfb->init(640, 480, 16);

    lfb->enableDoubleBuffering();

    lfb->drawPixel(0, 0, Colors::WHITE);

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    uint16_t xRes = lfb->getResX();

    uint16_t yRes = lfb->getResY();

    realBase = 4.0 / xRes;

    imaginaryBase = 4.0 / yRes;

    while (isRunning) {

        if (!shouldDraw) {

            continue;
        }

        shouldDraw = false;

        if (currentZoom < 1.0F) {

            currentZoom = 1.0F;
        }

        log.trace("Draw start");

        drawMandelbrot();

        log.trace("Draw end");

        lfb->show();
    }

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    Application::getInstance()->resume();
}

Color Mandelbrot::colorAt(uint32_t x, uint32_t y, double offsetX, double offsetY, double zoom) {

    Complex c((-2.0F + realBase * x) / zoom + offsetX, (-2.0F + imaginaryBase * y) / zoom + offsetY);

    Complex z(0.0, 0.0);

    for (uint32_t i = 0; i < MAX_VALUE; i++) {

        z = z * z + c;

        if (z.squaredAbsolute() >= 4.0) {

            return colors[i % MAX_COLORS];
        }
    }

    return Colors::BLACK;
}

void Mandelbrot::onEvent(const Event &event) {

    auto &keyEvent = (KeyEvent&) event;

    if (!keyEvent.getKey().isPressed()) {
        return;
    }

    bool hasChanged = false;

    switch (keyEvent.getKey().ascii()) {
        case '-':
            currentZoom -= currentZoom / 8;
            hasChanged = true;
            break;
        case '+':
            currentZoom += currentZoom / 8;
            hasChanged = true;
            break;
        default:
            break;
    }

    switch (keyEvent.getKey().scancode()) {
        case KeyEvent::UP:
            currentOffsetY -= 1 / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::DOWN:
            currentOffsetY += 1 / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::LEFT:
            currentOffsetX -= 1 / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::RIGHT:
            currentOffsetX += 1 / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::ESCAPE:
            isRunning = false;
            break;
        default:
            break;
    }

    if (hasChanged) {

        properties.ylim[0] = Y0_BASE / currentZoom + currentOffsetY;
        properties.ylim[1] = Y1_BASE / currentZoom + currentOffsetY;
        properties.ylim[0] = Y0_BASE / currentZoom + currentOffsetY;
        properties.ylim[1] = Y1_BASE / currentZoom + currentOffsetY;
        properties.xlim[0] = X0_BASE / currentZoom + currentOffsetX;
        properties.xlim[1] = X1_BASE / currentZoom + currentOffsetX;
        properties.xlim[0] = X0_BASE / currentZoom + currentOffsetX;
        properties.xlim[1] = X1_BASE / currentZoom + currentOffsetX;

        shouldDraw = true;
    }
}
