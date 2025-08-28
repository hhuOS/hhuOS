/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_TERMINAL_H
#define HHUOS_TERMINAL_H

#include <stdint.h>

#include "io/stream/FilterInputStream.h"
#include "io/stream/FilterOutputStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/stream/PipedOutputStream.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/stream/PipedInputStream.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/async/ReentrantSpinlock.h"

namespace Util {
namespace Io {
class KeyboardLayout;
}  // namespace Io
}  // namespace Util

namespace Util::Graphic {

class Terminal : public Util::Io::OutputStream, public Util::Io::InputStream {

public:

    enum Command {
        SET_ECHO,
        SET_LINE_AGGREGATION,
        SET_ANSI_PARSING,
        SET_CURSOR,
        ENABLE_RAW_MODE,
        ENABLE_CANONICAL_MODE,
        ENABLE_KEYBOARD_SCANCODES,
        SET_KEYBOARD_LAYOUT
    };

    Terminal(uint16_t columns, uint16_t rows);

    Terminal(const Terminal &copy) = delete;

    Terminal &operator=(const Terminal &other) = delete;

    ~Terminal() override = default;

    bool write(uint8_t c) override;

    uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    bool isReadyToRead() override;

    virtual void putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) = 0;

    virtual void clear(const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor, uint16_t startColumn, uint32_t startRow, uint16_t endColumn, uint16_t endRow) = 0;

    virtual void setPosition(uint16_t column, uint16_t row) = 0;

    [[nodiscard]] virtual uint16_t getCurrentColumn() const = 0;

    [[nodiscard]] virtual uint16_t getCurrentRow() const = 0;

    [[nodiscard]] uint16_t getColumns() const;

    [[nodiscard]] uint16_t getRows() const;

    [[nodiscard]] const Color& getForegroundColor() const;

    [[nodiscard]] const Color& getBackgroundColor() const;

    void clear();

    void setEcho(bool enabled);

    void setLineAggregation(bool enabled);

    void setAnsiParsing(bool enabled);

    void setKeyboardScancodes(bool enabled);

    void setKeyboardLayout(const Io::KeyboardLayout &layout);

    virtual void setCursor(bool enabled) = 0;

private:

    class TerminalStream : public Io::FilterInputStream, public Io::FilterOutputStream {

    public:
        /**
         * Constructor.
         */
        explicit TerminalStream(Terminal &terminal);

        /**
         * Copy Constructor.
         */
        TerminalStream(const TerminalStream &copy) = delete;

        /**
         * Assignment operator.
         */
        TerminalStream& operator=(const TerminalStream & other) = delete;

        /**
         * Destructor.
         */
        ~TerminalStream() override = default;

        bool write(uint8_t c) override;

        uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

        uint32_t flush() override;

    private:

        Terminal &terminal;
        Io::ByteArrayOutputStream lineBufferStream;

        Util::Io::PipedInputStream keyInputStream;
        Util::Io::PipedOutputStream keyOutputStream;
    };

    class KeyboardRunnable : public Async::Runnable {

    public:
        /**
         * Constructor.
         */
        explicit KeyboardRunnable(Terminal &terminal);

        /**
         * Copy Constructor.
         */
        KeyboardRunnable(const KeyboardRunnable &copy) = delete;

        /**
         * Assignment operator.
         */
        KeyboardRunnable& operator=(const KeyboardRunnable &other) = delete;

        /**
         * Destructor.
         */
        ~KeyboardRunnable() override = default;

        void run() override;

    private:

        Terminal &terminal;
    };

    static void handleBell();

    void handleTab();

    void parseColorEscapeSequence(const Util::String &escapeSequence);

    void parseCursorEscapeSequence(const Util::String &escapeSequence, char endCode);

    void parseEraseSequence(const Util::String &escapeSequence, char endCode);

    [[nodiscard]] static Util::Graphic::Color getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor, const Util::Array<Util::String> &codes, uint32_t &index);

    [[nodiscard]] static Util::Graphic::Color parseComplexColor(const Util::Array<Util::String> &codes, uint32_t &index);

    [[nodiscard]] static Util::Graphic::Color parse256Color(const Util::Array<Util::String> &codes, uint32_t &index);

    [[nodiscard]] static Util::Graphic::Color parseTrueColor(const Util::Array<Util::String> &codes, uint32_t &index);

    void parseGraphicRendition(uint8_t code);

    TerminalStream terminalStream;
    Util::Io::PipedInputStream ansiInputStream;
    Util::Io::PipedOutputStream ansiOutputStream;

    Io::KeyDecoder keyDecoder = Io::KeyDecoder(Io::DeLayout());
    Async::ReentrantSpinlock writeLock;

    Util::String currentEscapeSequence;
    bool isEscapeActive = false;

    Util::Graphic::Color foregroundBaseColor = Util::Graphic::Colors::WHITE;
    Util::Graphic::Color backgroundBaseColor = Util::Graphic::Colors::BLACK;
    Util::Graphic::Color foregroundColor = Util::Graphic::Colors::WHITE;
    Util::Graphic::Color backgroundColor = Util::Graphic::Colors::BLACK;
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

    const Util::String escapeEndCodes = Util::String::format("ABCDEFGHJKmnsu");

    static const constexpr uint8_t TABULATOR_SPACES = 8;
};

}

#endif
