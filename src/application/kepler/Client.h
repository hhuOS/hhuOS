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

#ifndef HHUOS_CLIENT_H
#define HHUOS_CLIENT_H

#include "util/collection/ArrayList.h"
#include "util/io/stream/FileInputStream.h"
#include "util/io/stream/FileOutputStream.h"

class ClientWindow;

class Client {

public:

    Client(size_t id, Util::Io::FileInputStream *inputStream, Util::Io::FileOutputStream *outputStream);

    ~Client();

    [[nodiscard]] Util::Io::FileInputStream& getInputStream() const;

    [[nodiscard]] Util::Io::FileOutputStream& getOutputStream() const;

    void addWindow(ClientWindow *window);

    [[nodiscard]] ClientWindow *getWindowById(size_t id) const;

private:

    size_t id;
    Util::ArrayList<ClientWindow*> windows;

    Util::Io::FileInputStream *inputStream = nullptr;
    Util::Io::FileOutputStream *outputStream = nullptr;
};

#endif