/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <device/cpu/Cpu.h>
#include <util/stream/TerminalOutputStream.h>
#include <util/stream/BufferedOutputStream.h>
#include <device/bios/Bios.h>
#include <device/graphic/VesaBiosExtensions.h>
#include <kernel/multiboot/MultibootLinearFrameBufferProvider.h>
#include <device/graphic/LinearFrameBufferTerminalProvider.h>
#include <device/graphic/ColorGraphicsArrayProvider.h>
#include <util/reflection/InstanceFactory.h>
#include <kernel/multiboot/Structure.h>
#include <kernel/multiboot/MultibootTerminalProvider.h>
#include <device/hid/Keyboard.h>
#include <util/stream/PipedInputStream.h>
#include <util/stream/PrintWriter.h>
#include <util/stream/InputStreamReader.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"

void GatesOfHell::enter() {
    if (Device::Bios::isAvailable()) {
        Device::Bios::init();
    }

    if (Device::Graphic::VesaBiosExtensions::isAvailable()) {
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::VesaBiosExtensions(true));
    }

    if (Device::Graphic::ColorGraphicsArrayProvider::isAvailable()) {
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::ColorGraphicsArrayProvider(true));
    }

    Device::Graphic::LinearFrameBufferProvider *lfbProvider = nullptr;
    Device::Graphic::TerminalProvider *terminalProvider;

    if (Kernel::Multiboot::Structure::hasKernelOption("lfb_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("lfb_provider");
        lfbProvider = reinterpret_cast<Device::Graphic::LinearFrameBufferProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (Kernel::Multiboot::MultibootLinearFrameBufferProvider::isAvailable()) {
        lfbProvider = new Kernel::Multiboot::MultibootLinearFrameBufferProvider();
    }

    if (Kernel::Multiboot::Structure::hasKernelOption("terminal_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("terminal_provider");
        terminalProvider = reinterpret_cast<Device::Graphic::TerminalProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (lfbProvider != nullptr) {
        terminalProvider = new Device::Graphic::LinearFrameBufferTerminalProvider(*lfbProvider);
    }  else if (Kernel::Multiboot::MultibootTerminalProvider::isAvailable()) {
        terminalProvider = new Kernel::Multiboot::MultibootTerminalProvider();
    } else {
        Device::Cpu::throwException(Device::Cpu::Exception::ILLEGAL_STATE, "Unable to find a suitable graphics driver for this machine!");
    }

    auto resolution = terminalProvider->searchMode(100, 37, 24);
    auto &terminal = terminalProvider->initializeTerminal(resolution);
    auto terminalStream = Util::Stream::TerminalOutputStream(terminal);
    auto bufferedStream = Util::Stream::BufferedOutputStream(terminalStream, resolution.columns);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    writer << "Welcome to hhuOS!" << Util::Stream::PrintWriter::endl
            << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")"
            << Util::Stream::PrintWriter::endl
            << "Git revision: " << BuildConfig::getGitRevision() << Util::Stream::PrintWriter::endl
            << "Build date: " << BuildConfig::getBuildDate() << Util::Stream::PrintWriter::endl;

    auto keyboardInputStream = Util::Stream::PipedInputStream();
    auto reader = Util::Stream::InputStreamReader(keyboardInputStream);
    auto keyboard = Device::Keyboard(keyboardInputStream);
    keyboard.plugin();

    while(true) {
        writer << reader.read() << Util::Stream::PrintWriter::flush;
    }
}