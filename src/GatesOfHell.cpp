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

#include <cstdint>
#include <device/cpu/Cpu.h>
#include <kernel/multiboot/Structure.h>
#include <util/graphic/Terminal.h>
#include <util/stream/TerminalOutputStream.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"

int32_t main() {
    GatesOfHell::enter();
}

void GatesOfHell::enter() {
    auto fbInfo = Kernel::Multiboot::Structure::getFrameBufferInfo();
    auto lfb = Util::Graphic::LinearFrameBuffer(fbInfo.address, fbInfo.width, fbInfo.height, fbInfo.bpp, fbInfo.pitch);
    auto terminal = Util::Graphic::Terminal(lfb);
    auto outputStream =  Util::Stream::TerminalOutputStream(terminal);

    outputStream << "Welcome to hhuOS!" << Util::Stream::OutputStream::endl
        << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")" << Util::Stream::OutputStream::endl
        << "Git revision: " << BuildConfig::getGitRevision() << Util::Stream::OutputStream::endl
        << "Build date: " << BuildConfig::getBuildDate() << Util::Stream::OutputStream::endl;

    Device::Cpu::halt();
}