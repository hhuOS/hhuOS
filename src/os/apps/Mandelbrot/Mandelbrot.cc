#include <kernel/services/GraphicsService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/EventBus.h>
#include <apps/Application.h>
#include "Mandelbrot.h"



Mandelbrot::Mandelbrot() : Thread("Mandelbrot") {

}

void Mandelbrot::run() {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    lfb->init(640, 480, 16);

    lfb->enableDoubleBuffering();

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    uint16_t xRes = lfb->getResX();

    uint16_t yRes = lfb->getResY();

    while (isRunning) {

        if (!shouldDraw) {

            continue;
        }

        shouldDraw = false;z

        if (currentZoom < 1.0F) {

            currentZoom = 1.0F;
        }

        for (uint16_t y = 0; y < yRes; y++) {

            for (uint16_t x = 0; x < xRes; x++) {

                lfb->drawPixel(x, y, colorAt(x, y, currentOffsetX, currentOffsetY, currentZoom * currentZoom));
            }
        }

        lfb->show();
    }

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    Application::getInstance()->resume();
}

Color Mandelbrot::colorAt(uint32_t x, uint32_t y, double offsetX, double offsetY, double zoom) {

    uint32_t count = 0;

    double real = (-2.0F + 4.0F / lfb->getResX() * x ) / zoom + offsetX;

    double imaginary = (-2.0F + 4.0F / lfb->getResY() * y) / zoom + offsetY;

    double zReal = 0.0;

    double zImaginary = 0.0;

    double lengthSq = 0.0;

    double tmp;

    do {

        tmp = zReal * zReal - zImaginary * zImaginary + real;

        zImaginary = 2 * zReal * zImaginary + imaginary;

        zReal = tmp;

        lengthSq = zReal * zReal + zImaginary * zImaginary;

        count++;

    } while ((lengthSq < 4.0) && (count < MAX_VALUE));

    return Color(count, 8);
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
