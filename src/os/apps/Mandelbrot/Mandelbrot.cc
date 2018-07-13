#include "Mandelbrot.h"
#include <kernel/services/GraphicsService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/EventBus.h>
#include <apps/Application.h>
#include <lib/math/Complex.h>

#include <kernel/cpu/CpuId.h>

extern void drawMandelbrotNoSSE();

__attribute__ ((aligned (16))) Mandelbrot::Properties Mandelbrot::properties;

__attribute__ ((aligned (16))) Mandelbrot::State Mandelbrot::state;

LinearFrameBuffer *Mandelbrot::lfb = nullptr;

ColorGradient Mandelbrot::gradient(MAX_COLORS);

Mandelbrot::Mandelbrot() : Thread("Mandelbrot"), log(Logger::get("Mandelbrot")) {
    useSSE = (CpuId::getFeatures() & CpuId::FEATURE_SSE2) == CpuId::FEATURE_SSE2;
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

        if(useSSE) {
            drawMandelbrotSSE();
        } else {
            drawMandelbrotNoSSE();
        }

        log.trace("Draw end");

        lfb->show();
    }

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    Application::getInstance()->resume();
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
