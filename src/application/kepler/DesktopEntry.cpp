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

#include "DesktopEntry.h"

#include "util/graphic/BitmapFile.h"
#include "util/io/file/File.h"

DesktopEntry::DesktopEntry(const DesktopFile &file) {
    const auto exec = file.getProperty("Exec");
    if (exec.isEmpty()) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "DesktopEntry: Exec property is empty!");
    }
    executable = exec.substring(0, exec.indexOf(" "));
    args = exec.substring(executable.length() + 1).split(" ");

    const auto iconProp = file.getProperty("Icon", DEFAULT_ICON_NAME);
    iconPath = iconProp.contains('/') ? iconProp : Util::String(ICON_PATH) + iconProp + ".bmp";

    name = file.getProperty("Name");
}

DesktopEntry::DesktopEntry(const Util::String &name, const Util::String &executable, const Util::Array<Util::String> &args, const Util::String &iconPath) :
    name(name), executable(executable), args(args), iconPath(iconPath) {}
