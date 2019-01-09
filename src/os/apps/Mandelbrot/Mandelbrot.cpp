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

#include "Mandelbrot.h"
#include <kernel/services/GraphicsService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/EventBus.h>
#include <apps/Application.h>
#include <lib/math/Complex.h>

#include <kernel/cpu/CpuId.h>

__attribute__ ((aligned (16))) Mandelbrot::Properties Mandelbrot::properties;

__attribute__ ((aligned (16))) Mandelbrot::State Mandelbrot::state;

LinearFrameBuffer *Mandelbrot::lfb = nullptr;

ColorGradient Mandelbrot::gradient(MAX_COLORS);

Mandelbrot::Mandelbrot() : Thread("Mandelbrot"), log(Logger::get("Mandelbrot")) {
    useSSE = (CpuId::getFeatures() & CpuId::FEATURE_SSE2) == CpuId::FEATURE_SSE2;
}

void Mandelbrot::run() {

    currentOffsetX = 0;

    currentOffsetY = 0;

    currentZoom = 1;

    calculatePosition();

    shouldDraw = true;

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    lfb->init(640, 480, 16);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(lfb->getDepth() == 1) {
        lfb->init(320, 200, 2);
    }

    lfb->enableDoubleBuffering();

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    uint16_t xRes = lfb->getResX();

    uint16_t yRes = lfb->getResY();

    globalOffsetX = static_cast<uint16_t>(xRes > 640 ? ((xRes - 640) / 2) : 0);

    globalOffsetY= static_cast<uint16_t>(yRes > 480 ? ((yRes - 480) / 2) : 0);

    realBase = 4.0f / xRes;

    imaginaryBase = 4.0f / yRes;

    while (isRunning) {

        if (!shouldDraw) {

            continue;
        }

        shouldDraw = false;

        if (currentZoom < 1.0F) {

            currentZoom = 1.0F;
        }

        if(useSSE) {
            drawMandelbrotSSE(properties.xlim, properties.ylim, globalOffsetX, globalOffsetY);
        } else {
            drawMandelbrotNoSSE(currentOffsetX, currentOffsetY, currentZoom);
        }

        lfb->show();
    }

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    Application::getInstance().resume();
}

void Mandelbrot::onEvent(const Event &event) {

    auto &keyEvent = (KeyEvent&) event;

    if (!keyEvent.getKey().isPressed()) {
        return;
    }

    bool hasChanged = false;

    switch (keyEvent.getKey().ascii()) {
        case '-':
            currentZoom -= currentZoom / ZOOM_FACTOR;
            hasChanged = true;
            break;
        case '+':
            currentZoom += currentZoom / ZOOM_FACTOR;
            hasChanged = true;
            break;
        default:
            break;
    }

    switch (keyEvent.getKey().scancode()) {
        case KeyEvent::UP:
            currentOffsetY -= OFFSET_STEP / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::DOWN:
            currentOffsetY += OFFSET_STEP / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::LEFT:
            currentOffsetX -= OFFSET_STEP / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::RIGHT:
            currentOffsetX += OFFSET_STEP / currentZoom;
            hasChanged = true;
            break;
        case KeyEvent::ESCAPE:
            isRunning = false;
            break;
        default:
            break;
    }

    if (hasChanged) {

        calculatePosition();

        shouldDraw = true;
    }
}

void Mandelbrot::calculatePosition() {
    properties.ylim[0] = Y0_BASE / currentZoom + currentOffsetY;
    properties.ylim[1] = Y1_BASE / currentZoom + currentOffsetY;
    properties.ylim[0] = Y0_BASE / currentZoom + currentOffsetY;
    properties.ylim[1] = Y1_BASE / currentZoom + currentOffsetY;
    properties.xlim[0] = X0_BASE / currentZoom + currentOffsetX;
    properties.xlim[1] = X1_BASE / currentZoom + currentOffsetX;
    properties.xlim[0] = X0_BASE / currentZoom + currentOffsetX;
    properties.xlim[1] = X1_BASE / currentZoom + currentOffsetX;
}
