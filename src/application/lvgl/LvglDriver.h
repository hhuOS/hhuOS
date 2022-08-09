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

#include <src/hal/lv_hal_disp.h>
#include "lib/util/graphic/LinearFrameBuffer.h"

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

private:

    void flush();

    static void flush(struct _lv_disp_drv_t * displayDriver, const lv_area_t * area, lv_color_t * pixels);

    uint32_t bufferSize;
    lv_disp_drv_t driver{};
    lv_disp_draw_buf_t displayBuffer{};
    lv_color_t *colorBuffer{};

    Util::Graphic::LinearFrameBuffer &lfb;
    Util::Memory::Address<uint32_t> &lfbAddress;
    Util::Memory::Address<uint32_t> colorBufferAddress;
    bool useMmx = false;
};

#endif
