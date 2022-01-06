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
#include <device/graphic/terminal/cga/ColorGraphicsAdapterProvider.h>
#include <lib/util/reflection/InstanceFactory.h>
#include <kernel/system/System.h>
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
#include <device/cpu/CpuId.h>
#include <lib/util/graphic/Ansi.h>
#include <lib/util/stream/FileOutputStream.h>
#include <kernel/service/MemoryService.h>
#include <lib/util/async/ThreadUtil.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"
#include "kernel/process/ProcessScheduler.h"
#include "lib/util/async/FunctionPointerRunnable.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/service/TimeService.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"

Kernel::Logger GatesOfHell::log = Kernel::Logger::get("GatesOfHell");
Util::Stream::InputStream *GatesOfHell::inputStream = nullptr;
Util::Stream::OutputStream *GatesOfHell::outputStream = nullptr;

void GatesOfHell::enter() {
    const auto logLevel = Kernel::Multiboot::Structure::hasKernelOption("log_level") ? Kernel::Multiboot::Structure::getKernelOption("log_level") : "info";
    Kernel::Logger::setLevel(logLevel);

    log.info("Welcome to hhuOS");
    log.info("%u MiB of physical memory detected", Kernel::System::getService<Kernel::MemoryService>().getPhysicalMemorySize() / 1024 / 1024);

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

    Device::SerialPort::initializeAvailablePorts();

    enableSerialLogging();

    Device::Keyboard::initialize();

    if (Kernel::Multiboot::Structure::hasKernelOption("headless_com_port")) {
        initializeHeadlessMode();
    } else {
        initializeTerminal();
        outputStream = new Util::Stream::FileOutputStream("/device/terminal");
        inputStream = new Util::Stream::FileInputStream("/device/keyboard");
    }

    printBanner();

    if (Kernel::Multiboot::Structure::hasKernelOption("color_test") && Kernel::Multiboot::Structure::getKernelOption("color_test") == "true") {
        colorTest();
    }

    if (Kernel::Multiboot::Structure::hasKernelOption("log_filesystem") && Kernel::Multiboot::Structure::getKernelOption("log_filesystem") == "true") {
        log.debug("Logging filesystem");
        listDirectory("/");
    }

    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();

    if (Kernel::Multiboot::Structure::hasKernelOption("scheduler_sign") && Kernel::Multiboot::Structure::getKernelOption("scheduler_sign") == "true") {
        auto *testRunnable = new Util::Async::FunctionPointerRunnable([](){
            auto lfb = Util::Graphic::LinearFrameBuffer(Util::File::File("/device/lfb"));
            auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
            auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);

            auto &font = Util::Graphic::Fonts::TERMINAL_FONT;
            auto &timeService = Kernel::System::getService<Kernel::TimeService>();
            const char *characters = "|/-\\";

            while (true) {
                auto time = timeService.getSystemTime();
                if (time.toMilliseconds() % 250 == 0) {
                    auto characterIndex = (time.toMilliseconds() % 1000) / 250;
                    stringDrawer.drawChar(font, lfb.getResolutionX() - font.getCharWidth(), 0, characters[characterIndex], Util::Graphic::Colors::RED, Util::Graphic::Colors::BLACK);
                }

                Util::Async::ThreadUtil::yield();
            }
        });

        auto &testThread = Kernel::Thread::createKernelThread("Test", testRunnable);
        Kernel::System::getService<Kernel::SchedulerService>().ready(testThread);
    }

    auto *shellRunnable = new Util::Async::FunctionPointerRunnable([](){
        auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
        auto writer = Util::Stream::PrintWriter(bufferedStream, true);
        auto reader = Util::Stream::InputStreamReader(*inputStream);

        writer << "> " << Util::Stream::PrintWriter::flush;

        while(true) {
            char input = reader.read();
            writer << input;

            if (input == '\n') {
                writer << "> ";
            }

            writer << Util::Stream::PrintWriter::flush;
        }
    });

    auto &shellThread = Kernel::Thread::createKernelThread("Shell", shellRunnable);
    schedulerService.ready(shellThread);

    log.info("Starting scheduler!");
    schedulerService.startScheduler();

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Once you entered the gates of hell, you are not allowed to leave!");
}

void GatesOfHell::initializeTerminal() {
    log.info("Initializing graphical terminal");

    if (Device::Graphic::VesaBiosExtensions::isAvailable()) {
        log.info("VESA graphics detected");
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::VesaBiosExtensions(true));
    }

    if (Device::Graphic::ColorGraphicsAdapterProvider::isAvailable()) {
        log.info("CGA graphics detected");
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::ColorGraphicsAdapterProvider(true));
    }

    Device::Graphic::LinearFrameBufferProvider *lfbProvider = nullptr;
    Device::Graphic::TerminalProvider *terminalProvider;

    if (Kernel::Multiboot::Structure::hasKernelOption("lfb_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("lfb_provider");
        log.info("LFB provider set to [%s] -> Starting initialization", static_cast<const char*>(providerName));
        lfbProvider = reinterpret_cast<Device::Graphic::LinearFrameBufferProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (Kernel::Multiboot::MultibootLinearFrameBufferProvider::isAvailable()) {
        log.info("LFB provider is not set -> Initializing LFB provider with multiboot values");
        lfbProvider = new Kernel::Multiboot::MultibootLinearFrameBufferProvider();
    }

    if (lfbProvider != nullptr) {
        auto mode = lfbProvider->searchMode(800, 600, 32);
        lfbProvider->initializeLinearFrameBuffer(mode, "lfb");
    }

    if (Kernel::Multiboot::Structure::hasKernelOption("terminal_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("terminal_provider");
        log.info("Terminal provider set to [%s] -> Starting initialization", static_cast<const char*>(providerName));
        terminalProvider = reinterpret_cast<Device::Graphic::TerminalProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else if (lfbProvider != nullptr) {
        log.info("Terminal provider is not set -> Initializing terminal provider with LFB");
        terminalProvider = new Device::Graphic::LinearFrameBufferTerminalProvider(Util::File::File("/device/lfb"));
    }  else if (Kernel::Multiboot::MultibootTerminalProvider::isAvailable()) {
        log.info("Terminal provider is not set and LFB is not available -> Initializing terminal with multiboot values");
        terminalProvider = new Kernel::Multiboot::MultibootTerminalProvider();
    } else {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Unable to find a suitable graphics driver for this machine!");
    }

    auto resolution = terminalProvider->searchMode(100, 37, 24);
    terminalProvider->initializeTerminal(resolution, "terminal");
}

void GatesOfHell::initializeHeadlessMode() {
    log.info("Headless mode enabled -> Initializing serial input/output");

    const auto port = Kernel::Multiboot::Structure::getKernelOption("log_com_port");
    const auto file = Util::File::File("/device/" + port.toLowerCase());
    if (!file.exists()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Headless mode: The requested serial port is not present!");
    }

    outputStream = new Util::Stream::FileOutputStream(file);
    inputStream = new Util::Stream::FileInputStream(file);
}

void GatesOfHell::enableSerialLogging() {
    if (!Kernel::Multiboot::Structure::hasKernelOption("log_com_port")) {
        return;
    }

    const auto port = Kernel::Multiboot::Structure::getKernelOption("log_com_port");
    const auto file = Util::File::File("/device/" + port.toLowerCase());
    if (!file.exists()) {
        log.error("Serial port [%s] not present", static_cast<const char*>(port));
        return;
    }

    auto *stream = new Util::Stream::FileOutputStream(file);
    Kernel::Logger::addOutputStream(*stream);
}

void GatesOfHell::initializeFilesystem() {
    log.info("Initializing filesystem");
    Kernel::System::registerService(Kernel::FilesystemService::SERVICE_ID, new Kernel::FilesystemService());
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();

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
    if (bannerFile.exists()) {
        auto bannerStream = Util::Stream::FileInputStream(bannerFile);
        auto bannerReader = Util::Stream::InputStreamReader(bannerStream);
        auto bufferedReader = Util::Stream::BufferedReader(bannerReader);

        auto banner = bufferedReader.read(bannerFile.getLength());
        writer << Util::Memory::String::format(static_cast<const char*>(banner),
                                               BuildConfig::getVersion(),
                                               BuildConfig::getBuildDate(),
                                               BuildConfig::getGitBranch(),
                                               BuildConfig::getGitRevision()) << Util::Stream::PrintWriter::endl;
    } else {
        printDefaultBanner(writer);
    }
}

void GatesOfHell::printDefaultBanner(Util::Stream::PrintWriter &writer) {
    writer << "Welcome to hhuOS!" << Util::Stream::PrintWriter::endl
           << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")" << Util::Stream::PrintWriter::endl
           << "Git revision: " << BuildConfig::getGitRevision() << Util::Stream::PrintWriter::endl
           << "Build date: " << BuildConfig::getBuildDate() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl;
}

void GatesOfHell::colorTest() {
    auto bufferedStream = Util::Stream::BufferedOutputStream(*outputStream);
    auto writer = Util::Stream::PrintWriter(bufferedStream, true);
    auto reader = Util::Stream::InputStreamReader(*inputStream);

    writer << Util::Graphic::Ansi::RESET << "4-bit colors:" << Util::Stream::PrintWriter::endl;

    for (uint32_t i = 0; i < 16; i++) {
        writer << Util::Graphic::Ansi::background8BitColor(i) << " ";
    }

    writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl << "8-bit colors:";

    for (uint32_t i = 0; i < 216; i++) {
        if (i % 36 == 0) {
            writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl;
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

    writer << Util::Graphic::Ansi::RESET << Util::Stream::PrintWriter::endl;
}

void GatesOfHell::listDirectory(const Util::Memory::String &path, uint32_t level) {
    const auto file = Util::File::File(path);
    if (!file.exists()) {
        return;
    }

    auto string = Util::Memory::String("|-");
    for (uint32_t i = 0; i < level; i++) {
        string += "-";
    }

    if (file.isDirectory()) {
        string += Util::Graphic::Ansi::BRIGHT_GREEN + file.getName() + "/" + Util::Graphic::Ansi::RESET;
    } else {
        string += Util::Graphic::Ansi::BRIGHT_YELLOW + file.getName() + Util::Graphic::Ansi::RESET;
    }

    log.debug(string);

    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            listDirectory(path + "/" + child, level + 1);
        }
    }
}
