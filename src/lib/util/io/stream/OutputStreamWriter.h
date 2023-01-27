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

#ifndef HHUOS_OUTPUTSTREAMWRITER_H
#define HHUOS_OUTPUTSTREAMWRITER_H

#include <cstdint>

#include "Writer.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Io {
class OutputStream;
}  // namespace Io
}  // namespace Util

namespace Util::Io {

class OutputStreamWriter : public Writer {

public:
    
    explicit OutputStreamWriter(OutputStream &outputStream);

    OutputStreamWriter(const OutputStreamWriter &copy) = delete;

    OutputStreamWriter &operator=(const OutputStreamWriter &copy) = delete;

    ~OutputStreamWriter() override = default;

    void flush() override;

    void write(char c) override;

    void write(const char *sourceBuffer, uint32_t length) override;

    void write(const char *sourceBuffer, uint32_t offset, uint32_t length) override;

    void write(const Util::String &string) override;

    void write(const Util::String &string, uint32_t offset, uint32_t length) override;

private:

    OutputStream &outputStream;

};

}

#endif
