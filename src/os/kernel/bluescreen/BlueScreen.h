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

#ifndef __BlueScreen_include__
#define __BlueScreen_include__

#include <kernel/threads/ThreadState.h>
#include <lib/OutputStream.h>
#include "kernel/memory/MemLayout.h"

class BlueScreen : public OutputStream {

public:

    BlueScreen(uint16_t columns, uint16_t rows);

    BlueScreen(const BlueScreen &other) = delete;

    BlueScreen &operator=(const BlueScreen &other) = delete;

    /**
     * Initializes the bluescreen.
     */
    virtual void initialize() = 0;

    /**
     * Prints out the bluescreen containing debug information.
     *
     * @param frame The interrupt frame.
     */
    void print(InterruptFrame &frame);

    void flush() override;

    static void setErrorMessage(const char *message);

protected:

    uint16_t x = 0;

    uint16_t y = 0;

    uint16_t columns, rows;

private:

    static const char *errorMessage;

private:

    virtual void show(uint16_t x, uint16_t y, char c) = 0;

    virtual void puts(const char *s, uint32_t n);

    virtual void putc(char c);
};

#endif
