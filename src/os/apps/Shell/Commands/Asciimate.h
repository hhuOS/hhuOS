/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, Jochen Peters
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

#ifndef HHUOS_ASCIIMATE_H
#define HHUOS_ASCIIMATE_H

#include <lib/file/File.h>
#include <kernel/services/TimeService.h>
#include "Command.h"

class Asciimate : public Command, Receiver {

private:

    bool isRunning = false;

    uint64_t fileLength = 0;

    char *asciimationBuffer = nullptr;

    TimeService *timeService = nullptr;

    LinearFrameBuffer *lfb = nullptr;

    Font &font = std_font_8x16;

    uint16_t posX = 0;
    uint16_t posY = 0;

    uint16_t rectWidth = 0;
    uint16_t rectHeight = 0;

    static const constexpr uint8_t FRAME_WIDTH = 68;
    static const constexpr uint8_t FRAME_HEIGHT = 13;

public:
    /**
     * Default-constructor.
     */
    Asciimate() = delete;

    /**
     * Copy-constructor.
     */
    Asciimate(const Asciimate &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Asciimate(Shell &shell);

    /**
     * Destructor.
     */
    ~Asciimate() override = default;

    /**
     * Play an asciimation file.
     */
    void play(File *file);

    /**
     * Draw a single asciimation frame.
     *
     * A frame is always 13 lines high 68 columns wide.
     *
     * @param frame Pointer to the frame
     */
    void drawFrame();

    /**
     * Read a single line from the asciimationBuffer into a given buffer.
     *
     * @param buf The buffer to read the line into
     */
    void readLine(char *buf);

    /**
     * Read the delay for the next frame in milliseconds from the asciimationBuffer.
     *
     * @return The delay for the next frame in milliseconds
     */
    uint32_t readDelay();

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;
};

#endif
