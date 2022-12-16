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

#ifndef HHUOS_FILEWRITER_H
#define HHUOS_FILEWRITER_H

#include <cstdint>

#include "Writer.h"
#include "lib/util/memory/String.h"

namespace Util {
namespace File {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Stream {

class FileWriter : public Writer {

public:

    explicit FileWriter(const File::File &file);

    explicit FileWriter(const Memory::String &path);

    FileWriter(const FileWriter &copy) = delete;

    FileWriter &operator=(const FileWriter &copy) = delete;

    ~FileWriter() override;

    void write(char c) override;

    void write(const char *sourceBuffer, uint32_t length) override;

    void write(const char *sourceBuffer, uint32_t offset, uint32_t length) override;

    void write(const Util::Memory::String &string) override;

    void write(const Util::Memory::String &string, uint32_t offset, uint32_t length) override;

private:

    uint32_t pos = 0;
    int32_t fileDescriptor;

};

}

#endif
