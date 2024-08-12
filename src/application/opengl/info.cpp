/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <GL/gl.h>

#include "lib/util/base/System.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/stream/PrintStream.h"

void info() {
    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Vendor: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << Util::Io::PrintStream::endl
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Renderer: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << Util::Io::PrintStream::endl
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Version: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << Util::Io::PrintStream::endl
                      << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Extensions: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
}