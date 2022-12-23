/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include <cstring>
#include <src/widgets/lv_img.h>
#include <src/core/lv_disp.h>
#include <src/core/lv_indev.h>

#include "lib/util/math/Math.h"
#include "lib/interface.h"
#include "lib/util/stream/FileInputStream.h"
#include "LvglDriver.h"
#include "lib/util/system/System.h"
#include "lib/util/async/Thread.h"
#include "lib/util/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/stream/InputStreamReader.h"

extern "C" {
uint64_t __udivdi3(uint64_t first, uint64_t second) {
    return static_cast<uint32_t>(first) / static_cast<uint32_t>(second);
}

uint64_t __umoddi3(uint64_t first, uint64_t second) {
    return static_cast<uint32_t>(first) % static_cast<uint32_t>(second);
}

void* malloc(uint32_t size) {
    return ::allocateMemory(size);
}

void free(void *pointer) {
    ::freeMemory(pointer);
}

void* realloc(void *pointer, uint32_t size) {
    return ::reallocateMemory(pointer, size);
}
}

uint32_t strlen(const char *str) noexcept {
    return Util::Memory::Address<uint32_t>(str).stringLength();
}

char* strcpy(char *dest, const char *src) noexcept {
    Util::Memory::Address<uint32_t>(dest).copyString(Util::Memory::Address<uint32_t>(src));
    return dest;
}

int32_t strcmp(const char *str1, const char *str2) noexcept {
    return Util::Memory::Address<uint32_t>(str1).compareString(Util::Memory::Address<uint32_t>(str2));
}

int32_t memcmp(const void *str1, const void *str2, uint32_t n) {
    return Util::Memory::Address<uint32_t>(str1).compareRange(Util::Memory::Address<uint32_t>(str2), n);
}

char* strcat(char *dest, const char *src) {
    auto targetAddress = Util::Memory::Address<uint32_t>(dest);
    targetAddress = targetAddress.add(targetAddress.stringLength());
    targetAddress.copyString(Util::Memory::Address<uint32_t>(src));
    return dest;
}

LvglDriver::LvglDriver(Util::Graphic::LinearFrameBuffer &lfb) :
        bufferSize(lfb.getPitch() * lfb.getResolutionY()),
        colorBuffer(new lv_color_t[bufferSize]), lfb(lfb),
        lfbAddress(*Util::Memory::Address<uint32_t>::createAcceleratedAddress(lfb.getBuffer().get(), useMmx)),
        colorBufferAddress(colorBuffer) {}

LvglDriver::~LvglDriver() {
    delete[] colorBuffer;
    delete &lfbAddress;
}

void LvglDriver::initialize() {
    lv_disp_draw_buf_init(&displayBuffer, colorBuffer, nullptr, lfb.getResolutionX() * lfb.getResolutionY());
    lv_disp_drv_init(&displayDriver);
    displayDriver.draw_buf = &displayBuffer;
    displayDriver.hor_res = static_cast<int16_t>(lfb.getResolutionX());
    displayDriver.ver_res = static_cast<int16_t>(lfb.getResolutionY());
    displayDriver.full_refresh = true;
    displayDriver.user_data = this;
    displayDriver.flush_cb = &flushDisplay;
    display = lv_disp_drv_register(&displayDriver);

    lv_indev_drv_init(&keyboardDriver);
    keyboardDriver.type = LV_INDEV_TYPE_KEYPAD;
    keyboardDriver.read_cb = &readKeyboardInput;
    keyboardDriver.user_data = this;
    keyboard = lv_indev_drv_register(&keyboardDriver);

    lv_indev_drv_init(&mouseDriver);
    mouseDriver.type = LV_INDEV_TYPE_POINTER;
    mouseDriver.read_cb = &readMouseInput;
    mouseDriver.user_data = this;
    mouse = lv_indev_drv_register(&mouseDriver);
    mouseState.x = lfb.getResolutionX() / 2;
    mouseState.y = lfb.getResolutionY() / 2;

    LV_IMG_DECLARE(cursor_icon);
    cursor = lv_img_create(lv_scr_act());
    lv_img_set_src(cursor, &cursor_icon);
    lv_indev_set_cursor(mouse, cursor);

    running = true;
    Util::Async::Thread::createThread("Keyboard-Listener", new KeyboardRunnable(*this));
    Util::Async::Thread::createThread("Mouse-Listener", new MouseRunnable(*this));
}

void LvglDriver::assignKeyboardToGroup(lv_group_t &group) {
    lv_indev_set_group(keyboard, &group);
}

void LvglDriver::flushDisplay(_lv_disp_drv_t *displayDriver, const lv_area_t *area, lv_color_t *pixels) {
    auto &driver = *reinterpret_cast<LvglDriver*>(displayDriver->user_data);
    driver.flush();
    lv_disp_flush_ready(displayDriver);
}

void LvglDriver::flush() {
    lfbAddress.copyRange(colorBufferAddress, bufferSize);
    if (useMmx) Util::Math::Math::endMmx();
}

void LvglDriver::readMouseInput(lv_indev_drv_t *mouseDriver, lv_indev_data_t *data) {
    auto &driver = *reinterpret_cast<LvglDriver*>(mouseDriver->user_data);

    driver.mouseLock.acquire();
    data->point.x = driver.mouseState.x;
    data->point.y = driver.mouseState.y;
    data->state = driver.mouseState.leftPressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    driver.mouseLock.release();
}

void LvglDriver::readKeyboardInput(lv_indev_drv_t *keyboardDriver, lv_indev_data_t *data) {
    auto &driver = *reinterpret_cast<LvglDriver*>(keyboardDriver->user_data);

    driver.keyboardLock.acquire();
    if (driver.keyboardEventQueue.isEmpty()) {
        driver.keyboardLock.release();
        return;
    }

    auto event = driver.keyboardEventQueue.poll();
    data->key = event.key;
    data->state = event.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->continue_reading = !driver.keyboardEventQueue.isEmpty();
    driver.keyboardLock.release();
}

bool LvglDriver::isRunning() const {
    return running;
}

LvglDriver::MouseRunnable::MouseRunnable(LvglDriver &driver) : driver(driver) {}

void LvglDriver::MouseRunnable::run() {
    auto file = Util::File::File("/device/mouse");
    if (!file.exists()) {
        return;
    }

    auto stream = Util::Stream::FileInputStream(file);

    while (true) {
        auto buttons = stream.read();
        auto xMovement = static_cast<int8_t>(stream.read());
        auto yMovement = static_cast<int8_t>(stream.read());

        driver.mouseLock.acquire();
        driver.mouseState.leftPressed = (buttons & 0x01) == 0x01;
        driver.mouseState.rightPressed = (buttons & 0x02) == 0x02;
        driver.mouseState.middlePressed = (buttons & 0x04) == 0x04;

        driver.mouseState.x += xMovement;
        driver.mouseState.y += yMovement;

        if (driver.mouseState.x < 0) driver.mouseState.x = 0;
        if (driver.mouseState.y < 0) driver.mouseState.y = 0;
        if (driver.mouseState.x >= driver.lfb.getResolutionX()) driver.mouseState.x = driver.lfb.getResolutionX() - 1;
        if (driver.mouseState.y >= driver.lfb.getResolutionY()) driver.mouseState.y = driver.lfb.getResolutionY() - 1;
        driver.mouseLock.release();
    }
}

LvglDriver::KeyboardRunnable::KeyboardRunnable(LvglDriver &driver) : driver(driver) {}

void LvglDriver::KeyboardRunnable::run() {
    while (true) {
        auto c = Util::System::in.read();

        if (c == '\n') {
            driver.running = false;
        }

        driver.keyboardLock.acquire();
        driver.keyboardEventQueue.offer(KeyboardEvent{c, true});
        driver.keyboardEventQueue.offer(KeyboardEvent{c, false});
        driver.keyboardLock.release();
    }
}

bool LvglDriver::KeyboardEvent::operator!=(const LvglDriver::KeyboardEvent &other) {
    return this != &other;
}
