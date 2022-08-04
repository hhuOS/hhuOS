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

#ifndef HHUOS_TERMINAL_H
#define HHUOS_TERMINAL_H

#include "lib/util/stream/OutputStream.h"
#include "lib/util/stream/InputStream.h"
#include "lib/util/memory/String.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/stream/PipedOutputStream.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/stream/ByteArrayOutputStream.h"

namespace Util::Graphic {

class Terminal : public Util::Stream::OutputStream, public Util::Stream::InputStream {

public:

    Terminal(uint16_t columns, uint16_t rows);

    Terminal(const Terminal &copy) = delete;

    Terminal &operator=(const Terminal &other) = delete;

    ~Terminal() override = default;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    int16_t read() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    virtual void putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) = 0;

    virtual void clear(const Util::Graphic::Color &backgroundColor) = 0;

    virtual void setPosition(uint16_t column, uint16_t row) = 0;

    [[nodiscard]] virtual uint16_t getCurrentColumn() const = 0;

    [[nodiscard]] virtual uint16_t getCurrentRow() const = 0;

    [[nodiscard]] uint16_t getColumns() const;

    [[nodiscard]] uint16_t getRows() const;

    [[nodiscard]] Util::Stream::PipedOutputStream& getPipedOutputStream();

private:

    class TerminalPipedOutputStream : public Util::Stream::PipedOutputStream {

    public:
        /**
         * Constructor.
         */
        explicit TerminalPipedOutputStream(Terminal &terminal, uint32_t lineBufferSize = LINE_BUFFER_SIZE);

        /**
         * Copy Constructor.
         */
        TerminalPipedOutputStream(const TerminalPipedOutputStream &copy) = delete;

        /**
         * Assignment operator.
         */
        TerminalPipedOutputStream& operator=(const TerminalPipedOutputStream & other) = delete;

        /**
         * Destructor.
         */
        ~TerminalPipedOutputStream() override;

        void write(uint8_t c) override;

        void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

        void flush() override;

    private:

        Terminal &terminal;
        Stream::ByteArrayOutputStream lineBufferStream;
        uint8_t *lineBuffer;

        static const constexpr uint32_t LINE_BUFFER_SIZE = 1024;
    };

    void parseColorEscapeSequence(const Util::Memory::String &escapeSequence);

    void parseCursorEscapeSequence(const Util::Memory::String &escapeSequence, char endCode);

    void parseEraseSequence(const Util::Memory::String &escapeSequence, char endCode);

    [[nodiscard]] static Util::Graphic::Color getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor, const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index);

    [[nodiscard]] static Util::Graphic::Color parseComplexColor(const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index);

    [[nodiscard]] static Util::Graphic::Color parse256Color(const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index);

    [[nodiscard]] static Util::Graphic::Color parseTrueColor(const Util::Data::Array<Util::Memory::String> &codes, uint32_t &index);

    void parseGraphicRendition(uint8_t code);

    Util::Stream::PipedInputStream inputStream;
    TerminalPipedOutputStream outputStream;

    Util::Memory::String currentEscapeSequence;
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

    const uint16_t columns;
    const uint16_t rows;

    uint16_t savedColumn = 0;
    uint16_t savedRow = 0;

    const Util::Memory::String escapeEndCodes = Util::Memory::String::format("ABCDEFGHJKmnsu");
};

}

#endif
