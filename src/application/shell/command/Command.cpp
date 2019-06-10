/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "lib/file/File.h"
#include "Command.h"

Command::Command(Shell &shell) :
        shell(shell),
        stdout(*File::open("/dev/stdout", "w")),
        stderr(*File::open("/dev/stderr", "w")),
        stdin(*(Kernel::getService<KernelStreamService>()->getStdin())) {

}

String Command::calcAbsolutePath(const String &relativePath) {
    if(relativePath.beginsWith(Filesystem::SEPARATOR)) {
        return relativePath;
    }

    String absolutePath = shell.getCurrentWorkingDirectory().getAbsolutePath();

    if (!absolutePath.endsWith(Filesystem::SEPARATOR)) {
        absolutePath += Filesystem::SEPARATOR;
    }

    absolutePath += relativePath;

    return absolutePath;
}
