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

#include <lib/util/stream/BufferedOutputStream.h>
#include <device/bios/Bios.h>
#include <device/graphic/lfb/vesa/VesaBiosExtensions.h>
#include <kernel/multiboot/MultibootLinearFrameBufferProvider.h>
#include <device/graphic/terminal/lfb/LinearFrameBufferTerminalProvider.h>
#include <device/graphic/terminal/cga/ColorGraphicsArrayProvider.h>
#include <lib/util/reflection/InstanceFactory.h>
#include <kernel/core/System.h>
#include <kernel/multiboot/Structure.h>
#include <kernel/multiboot/MultibootTerminalProvider.h>
#include <device/hid/Keyboard.h>
#include <lib/util/stream/PipedInputStream.h>
#include <lib/util/stream/InputStreamReader.h>
#include <lib/util/file/tar/Archive.h>
#include <filesystem/tar/ArchiveDriver.h>
#include <lib/util/file/File.h>
#include <lib/util/stream/BufferedReader.h>
#include <kernel/service/FilesystemService.h>
#include <filesystem/memory/MemoryDriver.h>
#include <lib/util/stream/FileInputStream.h>
#include <kernel/core/Management.h>
#include <device/cpu/CpuId.h>
#include <lib/util/graphic/Ansi.h>
#include <device/graphic/terminal/TerminalNode.h>
#include <lib/util/stream/FileOutputStream.h>
#include <kernel/core/SystemCall.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"

Kernel::Logger GatesOfHell::log = Kernel::Logger::get("GatesOfHell");
Util::Stream::InputStream *GatesOfHell::inputStream = nullptr;
Util::Stream::OutputStream *GatesOfHell::outputStream = nullptr;

void GatesOfHell::enter() {
    const auto logLevel = Kernel::Multiboot::Structure::hasKernelOption("log_level") ? Kernel::Multiboot::Structure::getKernelOption("log_level") : "info";
    Kernel::Logger::setLevel(logLevel);
    enableSerialLogging();

    log.info("Welcome to hhuOS");
    log.info("%u MiB of physical memory detected", Kernel::Management::getInstance().getTotalPhysicalMemory() / 1024 / 1024);

    if (Device::CpuId::isAvailable()) {
        log.info("CPU vendor: %s", static_cast<const char*>(Device::CpuId::getVendorString()));

        const auto info = Device::CpuId::getCpuInfo();
        log.info("CPU info: Family [%u], Model [%u], Stepping [%u], Type [%u]", info.family, info.model, info.stepping, info.type);

        const auto features = Device::CpuId::getCpuFeatures();
        Util::Memory::String featureString;
        for (uint32_t i = 0; i < features.length(); i++) {
            featureString += Device::CpuId::getFeatureAsString(features[i]);
            if (i < features.length() - 1) {
                featureString += ",";
            }
        }
        log.info("CPU features: %s", static_cast<const char*>(featureString));
    }

    if (Device::Bios::isAvailable()) {
        log.info("BIOS detected");
        Device::Bios::init();
    }

    initializeFilesystem();

    initializeKeyboard();

    if (Kernel::Multiboot::Structure::hasKernelOption("headless_com_port")) {
        initializeHeadlessMode();
    } else {
        initializeTerminal();
        outputStream = new Util::Stream::FileOutputStream("/device/terminal");
    }

    printBanner();

    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto reader = Util::Stream::InputStreamReader(*inputStream);

    if (Kernel::Multiboot::Structure::getKernelOption("color_test") == "true") {
        colorTest(writer);
    }

    Kernel::SystemCall::registerSystemCall(Util::System::SystemCall::SYSTEM_CALL_TEST, [](uint32_t paramCount, va_list params) -> Util::System::SystemCall::Result {
        return Util::System::SystemCall::OK;
    });

    const auto result = Util::System::SystemCall::execute(Util::System::SystemCall::SYSTEM_CALL_TEST, 0);

    writer << "> " << Util::Stream::PrintWriter::flush;

    while(true) {
        char input = reader.read();
        writer << input;

        if (input == '\n') {
            writer << "> ";
        }

        writer << Util::Stream::PrintWriter::flush;
    }
}

void GatesOfHell::initializeKeyboard() {
    log.info("Initializing keyboard");
    auto keyboardInputStream = new Util::Stream::PipedInputStream();
    auto keyboard = new Device::Keyboard(*keyboardInputStream);
    inputStream = keyboardInputStream;
    keyboard->plugin();
}

void GatesOfHell::initializeTerminal() {
    log.info("Initializing graphical terminal");

    if (Device::Graphic::VesaBiosExtensions::isAvailable()) {
        log.info("VESA graphics detected");
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::VesaBiosExtensions(true));
    }

    if (Device::Graphic::ColorGraphicsArrayProvider::isAvailable()) {
        log.info("CGA graphics detected");
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::ColorGraphicsArrayProvider(true));
    }

    Device::Graphic::LinearFrameBufferProvider *lfbProvider = nullptr;
    Device::Graphic::TerminalProvider *terminalProvider;

    if (Kernel::Multiboot::Structure::hasKernelOption("lfb_provider")) {
        log.info("LFB provider set to [%s] -> Starting initialization");
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("lfb_provider");
        lfbProvider = reinterpret_cast<Device::Graphic::LinearFrameBufferProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (Kernel::Multiboot::MultibootLinearFrameBufferProvider::isAvailable()) {
        log.info("LFB provider is not set -> Initializing LFB provider with multiboot values");
        lfbProvider = new Kernel::Multiboot::MultibootLinearFrameBufferProvider();
    }

    if (Kernel::Multiboot::Structure::hasKernelOption("terminal_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("terminal_provider");
        log.info("Terminal provider set to [%s] -> Starting initialization", static_cast<const char*>(providerName));
        terminalProvider = reinterpret_cast<Device::Graphic::TerminalProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (lfbProvider != nullptr) {
        log.info("Terminal provider is not set -> Initializing terminal provider with LFB");
        terminalProvider = new Device::Graphic::LinearFrameBufferTerminalProvider(*lfbProvider);
    }  else if (Kernel::Multiboot::MultibootTerminalProvider::isAvailable()) {
        log.info("Terminal provider is not set and LFB is not available -> Initializing terminal with multiboot values");
        terminalProvider = new Kernel::Multiboot::MultibootTerminalProvider();
    } else {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Unable to find a suitable graphics driver for this machine!");
    }

    auto resolution = terminalProvider->searchMode(100, 37, 24);
    auto &terminal = terminalProvider->initializeTerminal(resolution);

    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>()->getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    auto *terminalNode = new Device::Graphic::TerminalNode("terminal", terminal);
    driver.addNode("/", terminalNode);
}

void GatesOfHell::initializeHeadlessMode() {
    log.info("Headless mode enabled -> Initializing serial input/output");

    auto port = Device::SerialPort::portFromString(Kernel::Multiboot::Structure::getKernelOption("headless_com_port"));
    if (!Device::SerialPort::checkPort(port)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Headless mode: The requested serial port is not present!");
    } else {
        auto *serialInputStream = new Util::Stream::PipedInputStream();
        auto *serial = new Device::SerialPort(port, *serialInputStream);
        inputStream = serialInputStream;
        outputStream = new Device::SerialOutputStream(*serial);
        serial->plugin();
    }
}

void GatesOfHell::enableSerialLogging() {
    if (!Kernel::Multiboot::Structure::hasKernelOption("log_com_port")) {
        return;
    }

    auto port = Device::SerialPort::portFromString(Kernel::Multiboot::Structure::getKernelOption("log_com_port"));
    if (!Device::SerialPort::checkPort(port)) {
        log.error("No serial port present at %04x", port);
        return;
    }

    auto *comPort = new Device::SerialPort(port);
    auto *comStream = new Device::SerialOutputStream(*comPort);
    Kernel::Logger::addOutputStream(*comStream);
}

void GatesOfHell::initializeFilesystem() {
    log.info("Initializing filesystem");
    Kernel::System::registerService(Kernel::FilesystemService::SERVICE_NAME, *new Kernel::FilesystemService());
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>()->getFilesystem();

    log.info("Mounting root filesystem");
    auto *rootDriver = new Filesystem::Memory::MemoryDriver();
    filesystem.mountVirtualDriver("/", *rootDriver);

    auto *deviceDriver = new Filesystem::Memory::MemoryDriver();
    filesystem.createDirectory("/device");
    filesystem.mountVirtualDriver("/device", *deviceDriver);

    if (Kernel::Multiboot::Structure::isModuleLoaded("initrd")) {
        log.info("Initial ramdisk detected -> Mounting [%s]", "/initrd");
        auto module = Kernel::Multiboot::Structure::getModule("initrd");
        auto *tarArchive = new Util::File::Tar::Archive(module.start);
        auto *tarDriver = new Filesystem::Tar::ArchiveDriver(*tarArchive);

        filesystem.createDirectory("/initrd");
        filesystem.mountVirtualDriver("/initrd", *tarDriver);
    }
}

void GatesOfHell::printBanner() {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);

    auto bannerFile = Util::File::File("/initrd/banner.txt");
    if (!bannerFile.exists()) {
        printDefaultBanner(writer);
    } else {
        auto bannerStream = Util::Stream::FileInputStream(bannerFile);
        auto bannerReader = Util::Stream::InputStreamReader(bannerStream);
        auto bufferedReader = Util::Stream::BufferedReader(bannerReader);

        printBannerLine(writer, bufferedReader);
        writer << "# Welcome to " << Util::Graphic::Ansi::foreground24BitColor(Util::Graphic::Colors::HHU_BLUE) << Util::Graphic::Ansi::background24BitColor(Util::Graphic::Colors::WHITE.bright()) << "hhuOS" << Util::Graphic::Ansi::RESET << "!" << Util::Stream::PrintWriter::endl;
        printBannerLine(writer, bufferedReader);
        writer << "# Version      : " << BuildConfig::getVersion() << Util::Stream::PrintWriter::endl;
        printBannerLine(writer, bufferedReader);
        writer << "# Build Date   : " << BuildConfig::getBuildDate() << Util::Stream::PrintWriter::endl;
        printBannerLine(writer, bufferedReader);
        writer << "# Git Branch   : " << BuildConfig::getGitBranch() << Util::Stream::PrintWriter::endl;
        printBannerLine(writer, bufferedReader);
        writer << "# Git Commit   : " << BuildConfig::getGitRevision() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl;
    }
}

void GatesOfHell::printBannerLine(Util::Stream::PrintWriter &writer, Util::Stream::Reader &reader) {
    writer.write(Util::Graphic::Ansi::foreground24BitColor(Util::Graphic::Colors::HHU_BLUE));

    char c = reader.read();
    while (c != '\n') {
        writer << c;
        c = reader.read();
    }

    writer.write(Util::Graphic::Ansi::RESET);
}

void GatesOfHell::printDefaultBanner(Util::Stream::PrintWriter &writer) {
    writer << "Welcome to hhuOS!" << Util::Stream::PrintWriter::endl
           << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")" << Util::Stream::PrintWriter::endl
           << "Git revision: " << BuildConfig::getGitRevision() << Util::Stream::PrintWriter::endl
           << "Build date: " << BuildConfig::getBuildDate() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl;
}

void GatesOfHell::colorTest(Util::Stream::PrintWriter &writer) {
    writer << Util::Graphic::Ansi::RESET << "4-bit colors:" << Util::Stream::PrintWriter::endl;

    for (uint32_t i = 0; i < 16; i++) {
        writer << Util::Graphic::Ansi::background8BitColor(i) << " ";
    }

    writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl << "8-bit colors:";

    for (uint32_t i = 0; i < 216; i++) {
        if (i % 36 == 0) {
            writer << Util::Stream::PrintWriter::endl;
        }
        writer << Util::Graphic::Ansi::background8BitColor(i + 16) << " ";
    }

    writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl << "Grayscale colors:" << Util::Stream::PrintWriter::endl;

    for (uint32_t i = 232; i < 256; i++) {
        writer << Util::Graphic::Ansi::background8BitColor(i) << " ";
    }

    writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl << "24-bit colors:" << Util::Stream::PrintWriter::endl;

    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            for (uint32_t k = 0; k < 8; k++) {
                writer << Util::Graphic::Ansi::background24BitColor(Util::Graphic::Color(i * 32, j * 32, k * 32)) << " ";
            }
        }
        writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl;
    }

    writer << Util::Graphic::Ansi::RESET;
}
