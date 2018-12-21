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

#include <devices/IODeviceManager.h>
#include <filesystem/FileSystem.h>
#include <kernel/Kernel.h>
#include "SerialModule.h"
#include "SerialNode.h"
#include "BaudRateNode.h"

MODULE_PROVIDER {

    return new SerialModule();
};

int32_t SerialModule::initialize() {
    
    log = &Logger::get("SERIAL");

    auto *portService = Kernel::getService<PortService>();
    auto *fileSystem = Kernel::getService<FileSystem>();

    if(Serial::SerialDriver<Serial::COM1>::checkPort()) {
        log->info("Detected COM1");
        com1 = new Serial::SerialDriver<Serial::COM1>;

        IODeviceManager::getInstance().registerIODevice(com1);

        portService->registerPort(com1);

        fileSystem->createDirectory("/dev/ports/serial1");
        fileSystem->addVirtualNode("/dev/ports/serial1", new Serial::SerialNode<Serial::COM1>(com1));
        fileSystem->addVirtualNode("/dev/ports/serial1", new Serial::BaudRateNode<Serial::COM1>(com1));

        com1->plugin();
    }

    if(Serial::SerialDriver<Serial::COM2>::checkPort()) {
        log->info("Detected COM2");
        com2 = new Serial::SerialDriver<Serial::COM2>;

        IODeviceManager::getInstance().registerIODevice(com2);

        portService->registerPort(com2);

        fileSystem->createDirectory("/dev/ports/serial2");
        fileSystem->addVirtualNode("/dev/ports/serial2", new Serial::SerialNode<Serial::COM2>(com2));
        fileSystem->addVirtualNode("/dev/ports/serial2", new Serial::BaudRateNode<Serial::COM2>(com2));

        com2->plugin();
    }
    
    if(Serial::SerialDriver<Serial::COM3>::checkPort()) {
        log->info("Detected COM3");
        com3 = new Serial::SerialDriver<Serial::COM3>;

        IODeviceManager::getInstance().registerIODevice(com3);

        portService->registerPort(com3);

        fileSystem->createDirectory("/dev/ports/serial3");
        fileSystem->addVirtualNode("/dev/ports/serial3", new Serial::SerialNode<Serial::COM3>(com3));
        fileSystem->addVirtualNode("/dev/ports/serial3", new Serial::BaudRateNode<Serial::COM3>(com3));

        com3->plugin();
    }
    
    if(Serial::SerialDriver<Serial::COM4>::checkPort()) {
        log->info("Detected COM4");
        com4 = new Serial::SerialDriver<Serial::COM4>;

        IODeviceManager::getInstance().registerIODevice(com4);

        portService->registerPort(com4);

        fileSystem->createDirectory("/dev/ports/serial4");
        fileSystem->addVirtualNode("/dev/ports/serial4", new Serial::SerialNode<Serial::COM4>(com4));
        fileSystem->addVirtualNode("/dev/ports/serial4", new Serial::BaudRateNode<Serial::COM4>(com4));

        com4->plugin();
    }

    return 0;
}

int32_t SerialModule::finalize() {

    return 0;
}

String SerialModule::getName() {

    return "serial";
}

Util::Array<String> SerialModule::getDependencies() {

    return Util::Array<String>(0);
}