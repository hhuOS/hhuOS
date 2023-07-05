/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <src/widgets/lv_img.h>
#include <src/core/lv_disp.h>
#include <src/core/lv_indev.h>
#include <cstring>

#include "lib/util/math/Math.h"
#include "lib/interface.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "LvglDriver.h"
#include "lib/util/base/System.h"
#include "lib/util/async/Thread.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/key/Key.h"

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
    return Util::Address<uint32_t>(str).stringLength();
}

char* strcpy(char *dest, const char *src) noexcept {
    Util::Address<uint32_t>(dest).copyString(Util::Address<uint32_t>(src));
    return dest;
}

int32_t strcmp(const char *str1, const char *str2) noexcept {
    return Util::Address<uint32_t>(str1).compareString(Util::Address<uint32_t>(str2));
}

int32_t memcmp(const void *str1, const void *str2, uint32_t n) {
    return Util::Address<uint32_t>(str1).compareRange(Util::Address<uint32_t>(str2), n);
}

char* strcat(char *dest, const char *src) {
    auto targetAddress = Util::Address<uint32_t>(dest);
    targetAddress = targetAddress.add(targetAddress.stringLength());
    targetAddress.copyString(Util::Address<uint32_t>(src));
    return dest;
}

LvglDriver::LvglDriver(Util::Graphic::LinearFrameBuffer &lfb) :
        bufferSize(lfb.getPitch() * lfb.getResolutionY()),
        colorBuffer(new lv_color_t[bufferSize]), lfb(lfb),
        lfbAddress(*Util::Address<uint32_t>::createAcceleratedAddress(lfb.getBuffer().get(), useMmx)),
        colorBufferAddress(colorBuffer) {}

LvglDriver::~LvglDriver() {
    delete[] colorBuffer;
    delete &lfbAddress;
}

void LvglDriver::initialize() {
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

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
    if (useMmx) Util::Math::endMmx();
}

void LvglDriver::readMouseInput(lv_indev_drv_t *mouseDriver, lv_indev_data_t *data) {
    auto &driver = *reinterpret_cast<LvglDriver*>(mouseDriver->user_data);

    driver.mouseLock.acquire();
    data->point.x = driver.mouseState.x;
    data->point.y = driver.mouseState.y;
    data->state = driver.mouseState.buttons & Util::Io::Mouse::LEFT_BUTTON ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
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
    auto file = Util::Io::File("/device/mouse");
    if (!file.exists()) {
        return;
    }

    uint8_t values[4]{};
    auto stream = Util::Io::FileInputStream(file);

    while (driver.running) {
        values[0] = stream.read();
        values[1] = stream.read();
        values[2] = stream.read();
        values[3] = stream.read();

        auto mouseUpdate = Util::Io::MouseDecoder::decode(values);

        driver.mouseLock.acquire();
        driver.mouseState.buttons = mouseUpdate.buttons;
        driver.mouseState.x += mouseUpdate.xMovement;
        driver.mouseState.y -= mouseUpdate.yMovement;

        if (driver.mouseState.x < 0) driver.mouseState.x = 0;
        if (driver.mouseState.y < 0) driver.mouseState.y = 0;
        if (driver.mouseState.x >= driver.lfb.getResolutionX()) driver.mouseState.x = driver.lfb.getResolutionX() - 1;
        if (driver.mouseState.y >= driver.lfb.getResolutionY()) driver.mouseState.y = driver.lfb.getResolutionY() - 1;
        driver.mouseLock.release();
    }
}

LvglDriver::KeyboardRunnable::KeyboardRunnable(LvglDriver &driver) : driver(driver) {}

void LvglDriver::KeyboardRunnable::run() {
    auto keyDecoder = Util::Io::KeyDecoder();
    int16_t scancode = Util::System::in.read();

    while (driver.running && scancode != -1) {
        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();

            driver.keyboardLock.acquire();
            switch (key.getScancode()) {
                case Util::Io::Key::ESC :
                    if (key.isPressed()) driver.running = false;
                    break;
                default:
                    driver.keyboardEventQueue.offer(KeyboardEvent{key.getAscii(), key.isPressed()});
            }
            driver.keyboardLock.release();
        }

        scancode = Util::System::in.read();
    }
}

bool LvglDriver::KeyboardEvent::operator!=(const LvglDriver::KeyboardEvent &other) {
    return this != &other;
}
