/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_TERMINAL_H
#define HHUOS_LIB_UTIL_GRAPHIC_TERMINAL_H

#include <stdint.h>
#include <stddef.h>

#include "util/async/ReentrantSpinlock.h"
#include "util/async/Runnable.h"
#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/graphic/Color.h"
#include "util/graphic/Colors.h"
#include "util/io/stream/FilterInputStream.h"
#include "util/io/stream/FilterOutputStream.h"
#include "util/io/stream/OutputStream.h"
#include "util/io/stream/InputStream.h"
#include "util/io/stream/PipedOutputStream.h"
#include "util/io/stream/ByteArrayOutputStream.h"
#include "util/io/stream/PipedInputStream.h"
#include "util/io/key/layout/DeLayout.h"
#include "util/io/key/KeyDecoder.h"

namespace Util {
namespace Graphic {

/// Base class for terminal emulators.
/// It reads input from the keyboard stream ("/device/keyboard") and writes output via virtual methods.
/// It supports ANSI escape sequences for text formatting and cursor control.
/// Implementation must only handle the actual rendering of characters and screen manipulation.
class Terminal : public Io::OutputStream, public Io::InputStream {

public:
    /// Requests that can be issued to the terminal via `Util::Io::File::controlFile()` calls.
    /// These can be used to manipulate the terminal from user space.
    enum Command {
        /// Enable or disable echoing of typed characters.
        SET_ECHO,
        /// Enable or disable line aggregation (i.e. buffering until newline).
        SET_LINE_AGGREGATION,
        /// Enable or disable ANSI escape sequence parsing.
        SET_ANSI_PARSING,
        /// Enable or disable the terminal cursor.
        SET_CURSOR,
        /// Enable or disable raw mode (i.e. no line aggregation and no echo).
        ENABLE_RAW_MODE,
        /// Enable or disable canonical mode (i.e. line aggregation and echo).
        ENABLE_CANONICAL_MODE,
        /// Enable or disable keyboard scancodes instead of ascii characters.
        ENABLE_KEYBOARD_SCANCODES,
        /// Set the keyboard layout for decoding keyboard input.
        SET_KEYBOARD_LAYOUT
    };

    /// Create a new terminal instance with the given dimensions.
    Terminal(uint16_t columns, uint16_t rows);

    /// Print a character at the current cursor position with the specified colors.
    virtual void putChar(char c, const Color &foregroundColor, const Color &backgroundColor) = 0;

    /// Clear a rectangular area of the terminal with the specified colors.
    virtual void clear(const Color &foregroundColor, const Color &backgroundColor,
        uint16_t startColumn, uint16_t startRow, uint16_t endColumn, uint16_t endRow) = 0;

    /// Set the cursor position to the specified column and row.
    virtual void setPosition(uint16_t column, uint16_t row) = 0;

    /// Get the current cursor column.
    virtual uint16_t getCurrentColumn() const = 0;

    /// Get the current cursor row.
    virtual uint16_t getCurrentRow() const = 0;

    /// Enable or disable the terminal cursor.
    virtual void setCursorEnabled(bool enabled) = 0;

    /// Write a single byte to the terminal.
    bool write(uint8_t c) override;

    /// Write a buffer of bytes to the terminal.
    size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

    /// Read a single byte from the terminal.
    int16_t read() override;

    /// Peek at the next byte from the terminal without removing it from the input stream.
    int16_t peek() override;

    /// Read a buffer of bytes from the terminal.
    int32_t read(uint8_t *targetBuffer, size_t offset, size_t length) override;

    /// Check if there is data available to read from the terminal.
    bool isReadyToRead() override;
    
    /// Clear the entire terminal screen.
    void clear();

    /// Get the number of columns of the terminal.
    uint16_t getColumns() const {
        return columns;
    }

    /// Get the number of rows of the terminal.
    uint16_t getRows() const {
        return rows;
    }

    /// Get the current foreground color used for text rendering.
    const Color& getForegroundColor() const {
        return foregroundColor;
    }

    /// Get the current background color used for text rendering.
    const Color& getBackgroundColor() const {
        return backgroundColor;
    }

    /// Enable or disable echoing of typed characters.
    void setEcho(const bool enabled) {
        echo = enabled;
    }

    /// Enable or disable line aggregation (i.e. buffering until newline).
    void setLineAggregation(const bool enabled) {
        lineAggregation = enabled;
        terminalStream.flush();
    }

    /// Enable or disable ANSI escape sequence parsing.
    void setAnsiParsing(const bool enabled) {
        ansiParsing = enabled;
    }

    /// Enable or disable keyboard scancodes instead of ascii characters.
    void setKeyboardScancodes(const bool enabled) {
        keyboardScancodes = enabled;
    }

    /// Set the keyboard layout for decoding keyboard input.
    void setKeyboardLayout(const Io::KeyboardLayout &layout) {
        keyDecoder = Io::KeyDecoder(layout);
    }

private:

    class TerminalStream final : public Io::FilterInputStream, public Io::FilterOutputStream {

    public:

        explicit TerminalStream(Terminal &terminal);

        TerminalStream(const TerminalStream &copy) = delete;

        TerminalStream& operator=(const TerminalStream & other) = delete;

        ~TerminalStream() override = default;

        bool write(uint8_t c) override;

        size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

        uint32_t flush() override;

    private:

        Terminal &terminal;
        Io::ByteArrayOutputStream lineBufferStream;

        Io::PipedInputStream keyInputStream;
        Io::PipedOutputStream keyOutputStream;
    };

    class KeyboardRunnable final : public Async::Runnable {

    public:

        explicit KeyboardRunnable(Terminal &terminal);

        KeyboardRunnable(const KeyboardRunnable &copy) = delete;

        KeyboardRunnable& operator=(const KeyboardRunnable &other) = delete;

        ~KeyboardRunnable() override = default;

        void run() override;

    private:

        Terminal &terminal;
    };

    static void handleBell();

    void handleTab();

    void parseColorEscapeSequence(const String &escapeSequence);

    void parseCursorEscapeSequence(const String &escapeSequence, char endCode);

    void parseEraseSequence(const String &escapeSequence, char endCode);

    static Color getColor(uint8_t colorCode, const Color &defaultColor, const Array<String> &codes,
        size_t &index);

    static Color parseComplexColor(const Array<String> &codes, size_t &index);

    static Color parse256Color(const Array<String> &codes, size_t &index);

    static Color parseTrueColor(const Array<String> &codes, size_t &index);

    void parseGraphicRendition(uint8_t code);

    TerminalStream terminalStream;
    Io::PipedInputStream ansiInputStream;
    Io::PipedOutputStream ansiOutputStream;

    Io::KeyDecoder keyDecoder = Io::KeyDecoder(Io::DeLayout());
    Async::ReentrantSpinlock writeLock;

    String currentEscapeSequence;
    bool isEscapeActive = false;

    Color foregroundBaseColor = Colors::WHITE;
    Color backgroundBaseColor = Colors::BLACK;
    Color foregroundColor = Colors::WHITE;
    Color backgroundColor = Colors::BLACK;
    bool brightForeground = false;
    bool brightBackground = false;

    bool invert = false;
    bool bright = false;
    bool dim = false;

    bool echo = true;
    bool lineAggregation = true;
    bool ansiParsing = true;
    bool keyboardScancodes = false;

    const uint16_t columns;
    const uint16_t rows;

    uint16_t savedColumn = 0;
    uint16_t savedRow = 0;

    const String escapeEndCodes = "ABCDEFGHJKmnsu";

    static constexpr uint8_t TABULATOR_SPACES = 8;
};

}
}

#endif
