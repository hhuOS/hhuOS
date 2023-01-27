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

#ifndef HHUOS_FILEREADER_H
#define HHUOS_FILEREADER_H

#include <cstdint>

#include "Reader.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Io {

class FileReader : public Reader {

public:

    explicit FileReader(const Io::File &file);

    explicit FileReader(const String &path);

    FileReader(const FileReader &copy) = delete;

    FileReader &operator=(const FileReader &copy) = delete;

    ~FileReader() override;

    char read() override;

    int32_t read(char *targetBuffer, uint32_t length) override;

    int32_t read(char *targetBuffer, uint32_t offset, uint32_t length) override;

    String read(uint32_t length) override;

private:

    uint32_t pos = 0;
    int32_t fileDescriptor;

};

}

#endif
