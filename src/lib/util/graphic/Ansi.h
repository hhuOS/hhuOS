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

#ifndef __Ansi_include__
#define __Ansi_include__

#include <cstdint>

#include "lib/util/memory/String.h"
#include "Color.h"

namespace Util::Graphic {

class Ansi {

public:

    enum Color : uint8_t {
        BLACK = 0,
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        WHITE = 7,
    };

    enum GraphicRendition : uint8_t {
        NORMAL = 0,
        BRIGHT = 1,
        DIM = 2,
        ITALIC = 3,
        UNDERLINE = 4,
        SLOW_BLINK = 5,
        FAST_BLINK = 6,
        INVERT = 7,
        RESET_BRIGHT_DIM = 22,
        RESET_ITALIC = 23,
        RESET_UNDERLINE = 24,
        RESET_BLINK = 25,
        RESET_INVERT = 27
    };

    enum Key : int16_t {
        KEY_UP = 0x0100,
        KEY_DOWN = 0x0101,
        KEY_RIGHT = 0x0102,
        KEY_LEFT = 0x0103
    };

    struct CursorPosition {
        uint16_t column, row;
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Ansi() = delete;

    /**
     * Copy Constructor.
     */
    Ansi(const Ansi &other) = delete;

    /**
     * Assignment operator.
     */
    Ansi &operator=(const Ansi &other) = delete;

    /**
     * Destructor.
     */
    ~Ansi() = default;

    static void enableEcho();

    static void disableEcho();

    static void enableLineAggregation();

    static void disableLineAggregation();

    static void enableCursor();

    static void disableCursor();

    static void enableAnsiParsing();

    static void disableAnsiParsing();

    static void prepareGraphicalApplication(bool enableScancodes);

    static void cleanupGraphicalApplication();

    static void enableRawMode();

    static void enableCanonicalMode();

    [[nodiscard]] static Memory::String foreground8BitColor(uint8_t colorIndex);

    [[nodiscard]] static Memory::String background8BitColor(uint8_t colorIndex);

    [[nodiscard]] static Memory::String foreground24BitColor(const Graphic::Color &color);

    [[nodiscard]] static Memory::String background24BitColor(const Graphic::Color &color);

    static void setForegroundColor(Color color, bool bright);

    static void setBackgroundColor(Color color, bool bright);

    static void setForegroundColor(uint8_t colorIndex);

    static void setBackgroundColor(uint8_t colorIndex);

    static void setForegroundColor(const Graphic::Color &color);

    static void setBackgroundColor(const Graphic::Color &color);

    static void resetForegroundColor();

    static void resetBackgroundColor();

    static void resetColorsAndEffects();

    static void setPosition(const CursorPosition &position);

    static void moveCursorUp(uint16_t lines);

    static void moveCursorDown(uint16_t lines);

    static void moveCursorRight(uint16_t columns);

    static void moveCursorLeft(uint16_t columns);

    static void moveCursorToBeginningOfNextLine(uint16_t offset);

    static void moveCursorToBeginningOfPreviousLine(uint16_t offset);

    static void setColumn(uint16_t column);

    static void saveCursorPosition();

    static void restoreCursorPosition();

    static void clearScreen();

    static void clearScreenFromCursor();

    static void clearScreenToCursor();

    static void clearLine();

    static void clearLineFromCursor();

    static void clearLineToCursor();

    [[nodiscard]] static CursorPosition getCursorPosition();

    [[nodiscard]] static CursorPosition getCursorLimits();

    [[nodiscard]] static int16_t readChar();

    static const constexpr char ESCAPE_SEQUENCE_START = 0x1b;
    static const constexpr char *RESET = "\u001b[0m";
    static const constexpr char *FOREGROUND_BLACK = "\u001b[30m";
    static const constexpr char *FOREGROUND_RED = "\u001b[31m";
    static const constexpr char *FOREGROUND_GREEN = "\u001b[32m";
    static const constexpr char *FOREGROUND_YELLOW = "\u001b[33m";
    static const constexpr char *FOREGROUND_BLUE = "\u001b[34m";
    static const constexpr char *FOREGROUND_MAGENTA = "\u001b[35m";
    static const constexpr char *FOREGROUND_CYAN = "\u001b[36m";
    static const constexpr char *FOREGROUND_WHITE = "\u001b[37m";
    static const constexpr char *FOREGROUND_DEFAULT = "\u001b[39m";
    static const constexpr char *FOREGROUND_BRIGHT_BLACK = "\u001b[90m";
    static const constexpr char *FOREGROUND_BRIGHT_RED = "\u001b[91m";
    static const constexpr char *FOREGROUND_BRIGHT_GREEN = "\u001b[92m";
    static const constexpr char *FOREGROUND_BRIGHT_YELLOW = "\u001b[93m";
    static const constexpr char *FOREGROUND_BRIGHT_BLUE = "\u001b[94m";
    static const constexpr char *FOREGROUND_BRIGHT_MAGENTA = "\u001b[95m";
    static const constexpr char *FOREGROUND_BRIGHT_CYAN = "\u001b[96m";
    static const constexpr char *FOREGROUND_BRIGHT_WHITE = "\u001b[97m";
    static const constexpr char *BACKGROUND_BLACK = "\u001b[40m";
    static const constexpr char *BACKGROUND_RED = "\u001b[41m";
    static const constexpr char *BACKGROUND_GREEN = "\u001b[42m";
    static const constexpr char *BACKGROUND_YELLOW = "\u001b[43m";
    static const constexpr char *BACKGROUND_BLUE = "\u001b[44m";
    static const constexpr char *BACKGROUND_MAGENTA = "\u001b[45m";
    static const constexpr char *BACKGROUND_CYAN = "\u001b[46m";
    static const constexpr char *BACKGROUND_WHITE = "\u001b[47m";
    static const constexpr char *BACKGROUND_DEFAULT = "\u001b[49m";
    static const constexpr char *BACKGROUND_BRIGHT_BLACK = "\u001b[100m";
    static const constexpr char *BACKGROUND_BRIGHT_RED = "\u001b[101m";
    static const constexpr char *BACKGROUND_BRIGHT_GREEN = "\u001b[102m";
    static const constexpr char *BACKGROUND_BRIGHT_YELLOW = "\u001b[103m";
    static const constexpr char *BACKGROUND_BRIGHT_BLUE = "\u001b[104m";
    static const constexpr char *BACKGROUND_BRIGHT_MAGENTA = "\u001b[105m";
    static const constexpr char *BACKGROUND_BRIGHT_CYAN = "\u001b[106m";
    static const constexpr char *BACKGROUND_BRIGHT_WHITE = "\u001b[107m";

    static const Graphic::Color colorTable256[256];

private:

    static const Util::Memory::String escapeEndCodes;

};

}

#endif
