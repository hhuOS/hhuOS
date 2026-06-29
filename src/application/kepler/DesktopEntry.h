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

#ifndef HHUOS_DESKTOPENTRY_H
#define HHUOS_DESKTOPENTRY_H

#include <util/base/String.h>
#include <util/graphic/Image.h>

class DesktopEntry {

public:

    DesktopEntry() = default;

    DesktopEntry(const Util::String &name, const Util::String &executable, const Util::Array<Util::String> &args, const Util::String &iconPath);

    DesktopEntry(const DesktopEntry &other) = delete;

    DesktopEntry& operator=(const DesktopEntry &other) = delete;

    ~DesktopEntry() {
        delete icon;
    }

    const Util::String& getName() const {
        return name;
    }

    const Util::String& getExecutable() const {
        return executable;
    }

    const Util::Array<Util::String>& getArgs() const {
        return args;
    }

    Util::Graphic::Image& getIcon() const {
        return *icon;
    }

    static constexpr int32_t ICON_SIZE = 32;

private:

    Util::String name;
    Util::String executable;
    Util::Array<Util::String> args;
    Util::Graphic::Image *icon = nullptr;

    static constexpr const char *DEFAULT_ICON = "/user/kepler/telescope.bmp";
};

#endif
