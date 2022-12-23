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

#ifndef HHUOS_LVGLDRIVER_H
#define HHUOS_LVGLDRIVER_H

#include <cstdint>
#include <src/hal/lv_hal_disp.h>
#include <src/core/lv_group.h>
#include <src/core/lv_obj.h>
#include <src/hal/lv_hal_indev.h>
#include <src/misc/lv_area.h>
#include <src/misc/lv_color.h>

#include "lib/util/async/Runnable.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/ArrayListBlockingQueue.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/Address.h"

namespace Util {
namespace Graphic {
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

class LvglDriver {

public:
    /**
     * Constructor.
     */
    LvglDriver(Util::Graphic::LinearFrameBuffer &lfb);

    /**
     * Copy Constructor.
     */
    LvglDriver(const LvglDriver &other) = delete;

    /**
     * Assignment operator.
     */
    LvglDriver &operator=(const LvglDriver &other) = delete;

    /**
     * Destructor.
     */
    ~LvglDriver();

    void initialize();

    void assignKeyboardToGroup(lv_group_t &group);

    [[nodiscard]] bool isRunning() const;

private:

    struct MouseState {
        bool leftPressed = false;
        bool rightPressed = false;
        bool middlePressed = false;
        int16_t x = 0;
        int16_t y = 0;
    };

    struct KeyboardEvent {
        char key = 0;
        bool pressed = false;

        bool operator!=(const KeyboardEvent &other);
    };

    class MouseRunnable : public Util::Async::Runnable {

    public:
        /**
         * Constructor.
         */
        MouseRunnable(LvglDriver &driver);

        /**
         * Copy Constructor.
         */
        MouseRunnable(const MouseRunnable &other) = delete;

        /**
         * Assignment operator.
         */
        MouseRunnable &operator=(const MouseRunnable &other) = delete;

        /**
         * Destructor.
         */
        ~MouseRunnable() = default;

        void run() override;

    private:

        LvglDriver &driver;
    };

    class KeyboardRunnable : public Util::Async::Runnable {

    public:
        /**
         * Constructor.
         */
        KeyboardRunnable(LvglDriver &driver);

        /**
         * Copy Constructor.
         */
        KeyboardRunnable(const KeyboardRunnable &other) = delete;

        /**
         * Assignment operator.
         */
        KeyboardRunnable &operator=(const KeyboardRunnable &other) = delete;

        /**
         * Destructor.
         */
        ~KeyboardRunnable() = default;

        void run() override;

    private:

        LvglDriver &driver;
    };

    void flush();

    static void flushDisplay(_lv_disp_drv_t *displayDriver, const lv_area_t *area, lv_color_t *pixels);

    static void readMouseInput(lv_indev_drv_t *mouseDriver, lv_indev_data_t *data);

    static void readKeyboardInput(lv_indev_drv_t *keyboardDriver, lv_indev_data_t *data);

    uint32_t bufferSize;
    lv_disp_drv_t displayDriver{};
    lv_disp_draw_buf_t displayBuffer{};
    lv_color_t *colorBuffer = nullptr;
    lv_disp_t *display = nullptr;

    lv_indev_drv_t mouseDriver{};
    lv_indev_t *mouse = nullptr;
    lv_obj_t *cursor = nullptr;
    MouseState mouseState;
    Util::Async::Spinlock mouseLock;

    lv_indev_drv_t keyboardDriver{};
    lv_indev_t *keyboard = nullptr;
    Util::Data::ArrayListBlockingQueue<KeyboardEvent> keyboardEventQueue;
    Util::Async::Spinlock keyboardLock;

    Util::Graphic::LinearFrameBuffer &lfb;
    Util::Memory::Address<uint32_t> &lfbAddress;
    Util::Memory::Address<uint32_t> colorBufferAddress;
    bool useMmx = false;

    bool running = false;
};

#endif
