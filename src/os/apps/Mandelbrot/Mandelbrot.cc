#include "Mandelbrot.h"
#include <kernel/services/GraphicsService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/EventBus.h>
#include <apps/Application.h>
#include <lib/math/Complex.h>

#include <xmmintrin.h>

struct spec {
    /* Image Specification */
    int width;
    int height;
    int depth;
    /* Fractal Specification */
    float xlim[2];
    float ylim[2];
    int iterations;
};

typedef __m128 __m128a __attribute__ ((aligned (16)));

struct MandelbrotState {
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
} ;

MandelbrotState mandelbrotState;

void mandel_sse2(unsigned char *image, const struct spec *s) {

    Cpu::disableInterrupts();

    mandelbrotState.xmin = _mm_set_ps1(1.0F);
    mandelbrotState.ymin = _mm_set_ps1(2.0F);

    __m128a r = _mm_mul_ps(mandelbrotState.xmin, mandelbrotState.ymin);

//    mandelbrotState.xmin = _mm_set_ps1(s->xlim[0]);
//    mandelbrotState.ymin = _mm_set_ps1(s->ylim[0]);
//    mandelbrotState.xscale = _mm_set_ps1((s->xlim[1] - s->xlim[0]) / s->width);
//    mandelbrotState.yscale = _mm_set_ps1((s->ylim[1] - s->ylim[0]) / s->height);
//    mandelbrotState.threshold = _mm_set_ps1(4);
//    mandelbrotState.one = _mm_set_ps1(1);
//    mandelbrotState.iter_scale = _mm_set_ps1(1.0f / s->iterations);
//    mandelbrotState.depth_scale = _mm_set_ps1(s->depth - 1);
//
//    mandelbrotState.depth_scale = _mm_mul_ps(mandelbrotState.xmin, mandelbrotState.ymin);

//    for (int y = 0; y < s->height; y++) {
//        for (int x = 0; x < s->width; x += 4) {
//            mandelbrotState.mx = _mm_set_ps(x + 3, x + 2, x + 1, x + 0);
//            mandelbrotState.my = _mm_set_ps1(y);
//            mandelbrotState.cr = _mm_add_ps(_mm_mul_ps(mandelbrotState.mx, mandelbrotState.xscale), mandelbrotState.xmin);
//            mandelbrotState.ci = _mm_add_ps(_mm_mul_ps(mandelbrotState.my, mandelbrotState.yscale), mandelbrotState.ymin);
//            mandelbrotState.zr = mandelbrotState.cr;
//            mandelbrotState.zi = mandelbrotState.ci;
//            int k = 1;
//            mandelbrotState.mk = _mm_set_ps1(k);
//            while (++k < s->iterations) {
//                /* Compute z1 from z0 */
//                mandelbrotState.zr2 = _mm_mul_ps(mandelbrotState.zr, mandelbrotState.zr);
//                mandelbrotState.zi2 = _mm_mul_ps(mandelbrotState.zi, mandelbrotState.zi);
//                mandelbrotState.zrzi = _mm_mul_ps(mandelbrotState.zr, mandelbrotState.zi);
//                mandelbrotState.zr = _mm_add_ps(_mm_sub_ps(mandelbrotState.zr2, mandelbrotState.zi2), mandelbrotState.cr);
//                mandelbrotState.zi = _mm_add_ps(_mm_add_ps(mandelbrotState.zrzi, mandelbrotState.zrzi), mandelbrotState.ci);
//
//                /* Increment k */
//                mandelbrotState.zr2 = _mm_mul_ps(mandelbrotState.zr, mandelbrotState.zr);
//                mandelbrotState.zi2 = _mm_mul_ps(mandelbrotState.zi, mandelbrotState.zi);
//                mandelbrotState.mag2 = _mm_add_ps(mandelbrotState.zr2, mandelbrotState.zi2);
//                mandelbrotState.mask = _mm_cmplt_ps(mandelbrotState.mag2, mandelbrotState.threshold);
//                mandelbrotState.mk = _mm_add_ps(_mm_and_ps(mandelbrotState.mask, mandelbrotState.one), mandelbrotState.mk);
//
//                /* Early bailout? */
//                if (_mm_movemask_ps(mandelbrotState.mask) == 0)
//                    break;
//            }
//            mandelbrotState.mk = _mm_mul_ps(mandelbrotState.mk, mandelbrotState.iter_scale);
//            mandelbrotState.mk = _mm_sqrt_ps(mandelbrotState.mk);
//            mandelbrotState.mk = _mm_mul_ps(mandelbrotState.mk, mandelbrotState.depth_scale);
//            mandelbrotState.pixels = _mm_cvtps_epi32(mandelbrotState.mk);
//            unsigned char *dst = image + y * s->width * 3 + x * 3;
//            unsigned char *src = (unsigned char *)&mandelbrotState.pixels;
//            for (int i = 0; i < 4; i++) {
//                dst[i * 4 + 0] = 0x0;
//                dst[i * 4 + 1] = src[i * 4];
//                dst[i * 4 + 2] = src[i * 4];
//                dst[i * 4 + 3] = src[i * 4];
//            }
//        }
//    }
}

void Mandelbrot::mandel() {

    struct spec spec = {
            .width = 640,
            .height = 480,
            .depth = 256,
            .xlim = {-2.5, 1.5},
            .ylim = {-1.5, 1.5},
            .iterations = 256
    };

    unsigned char *image = (unsigned char*) malloc(spec.width * spec.height * 4);

    log.trace("Calculate start");

    mandel_sse2(image, &spec);

    log.trace("Calculate end");

    uint32_t *iterator = (uint32_t*) image;

    log.trace("Draw start");

//    for (uint32_t i = 0; i < 640 * 480; i++) {
//
//        lfb->drawPixel(i % 640, i / 640, Color(iterator[i], 32));
//    }

    log.trace("Draw end");
}

Color Mandelbrot::colors[MAX_COLORS] {
    Color(77, 157, 224),
    Color(255, 85, 84),
    Color(255, 188, 41),
    Color(59, 178, 115),
    Color(119, 104, 174)
};

Mandelbrot::Mandelbrot() : Thread("Mandelbrot"), log(Logger::get("Mandelbrot")) {

}

void Mandelbrot::run() {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    lfb->init(640, 480, 16);

    mandel();

    lfb->drawPixel(0, 0, Colors::WHITE);

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    while(isRunning);

//    uint16_t xRes = lfb->getResX();
//
//    uint16_t yRes = lfb->getResY();
//
//    realBase = 4.0 / xRes;
//
//    imaginaryBase = 4.0 / yRes;
//
//    while (isRunning) {
//
//        if (!shouldDraw) {
//
//            continue;
//        }
//
//        shouldDraw = false;
//
//        if (currentZoom < 1.0F) {
//
//            currentZoom = 1.0F;
//        }
//
//        for (uint16_t y = 0; y < yRes; y++) {
//
//            for (uint16_t x = 0; x < xRes; x++) {
//
//                lfb->drawPixel(x, y, colorAt(x, y, currentOffsetX, currentOffsetY, currentZoom * currentZoom));
//            }
//        }
//
//        lfb->show();
//    }

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

    switch (keyEvent.getKey().ascii()) {
        case '-':
            shouldDraw = true;
            currentZoom -= currentZoom / 8;
            break;
        case '+':
            shouldDraw = true;
            currentZoom += currentZoom / 8;
            break;
        default:
            break;
    }

    switch (keyEvent.getKey().scancode()) {
        case KeyEvent::UP:
            shouldDraw = true;
            currentOffsetY -= 0.1F / (currentZoom * currentZoom);
            break;
        case KeyEvent::DOWN:
            shouldDraw = true;
            currentOffsetY += 0.1F / (currentZoom * currentZoom);
            break;
        case KeyEvent::LEFT:
            shouldDraw = true;
            currentOffsetX -= 0.1F / (currentZoom * currentZoom);
            break;
        case KeyEvent::RIGHT:
            shouldDraw = true;
            currentOffsetX += 0.1F / (currentZoom * currentZoom);
            break;
        case KeyEvent::ESCAPE:
            isRunning = false;
            break;
        default:
            break;
    }

}
