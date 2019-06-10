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

#include "kernel/thread/ThreadState.h"
#include "lib/stream/OutputStream.h"
#include "kernel/memory/MemLayout.h"

/**
 * Interface for an error screen, that is shown after a system crash has occurred (called "bluescreen").
 *
 * An error messages, the current stack frame and the state of the CPU registers are shown.
 *
 * @author Filip Krakowski, Fabian Ruhland
 * @date 2018
 */
class BlueScreen : public OutputStream {

public:

    /**
     * Constructor.
     *
     * @param columns Amount of the screen's columns
     * @param rows Amount of the screen's rows
     */
    BlueScreen(uint16_t columns, uint16_t rows);

    /**
     * Copy-constructor.
     */
    BlueScreen(const BlueScreen &other) = delete;

    /**
     * Assignment operator.
     */
    BlueScreen &operator=(const BlueScreen &other) = delete;

    /**
     * Initialize the bluescreen
     */
    virtual void initialize() = 0;

    /**
     * Take debug information from an interrupt frame and print it.
     *
     * @param frame The interrupt frame.
     */
    void print(InterruptFrame &frame);

    /**
     * Overriding function from OutputStream.
     */
    void flush() override;

    /**
     * Set the error message to be printed.
     * This should be a short one-line message.
     */
    static void setErrorMessage(const char *message);

protected:

    uint16_t x = 0;

    uint16_t y = 0;

    uint16_t columns, rows;

private:

    static const char *errorMessage;

private:

    /**
     * Show a character anywhere on the screen.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param c The character to show
     */
    virtual void show(uint16_t x, uint16_t y, char c) = 0;

    /**
     * Print a character.
     *
     * @param c The character
     */
    void putc(char c);

    /**
     * Print a string.
     *
     * @param s The string
     * @param n The string's length
     */
    void puts(const char *s, uint32_t n);
};

#endif
