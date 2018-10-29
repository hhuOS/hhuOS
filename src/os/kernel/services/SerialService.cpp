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

#include <kernel/threads/Scheduler.h>
#include <devices/IODeviceManager.h>
#include "SerialService.h"

Logger &SerialService::log = Logger::get("SERIAL");

SerialService::SerialService() {
    if(Serial::checkPort(Serial::COM1)) {
        log.info("Detected COM1");
        com1 = new Serial(Serial::COM1);

        IODeviceManager::getInstance().registerIODevice(com1);
    }

    if(Serial::checkPort(Serial::COM2)) {
        log.info("Detected COM2");
        com2 = new Serial(Serial::COM2);

        IODeviceManager::getInstance().registerIODevice(com2);
    }

    if(Serial::checkPort(Serial::COM3)) {
        log.info("Detected COM3");
        com3 = new Serial(Serial::COM3);

        IODeviceManager::getInstance().registerIODevice(com3);
    }

    if(Serial::checkPort(Serial::COM4)) {
        log.info("Detected COM4");
        com4 = new Serial(Serial::COM4);

        IODeviceManager::getInstance().registerIODevice(com4);
    }
}

Serial *SerialService::getSerialPort(Serial::ComPort port) {
    switch(port) {
        case Serial::COM1 :
            return com1;
        case Serial::COM2 :
            return com2;
        case Serial::COM3 :
            return com3;
        case Serial::COM4 :
            return com4;
        default:
            return nullptr;
    }
}

bool SerialService::isPortAvailable(Serial::ComPort port) {
    switch(port) {
        case Serial::COM1 :
            return com1 != nullptr;
        case Serial::COM2 :
            return com2 != nullptr;
        case Serial::COM3 :
            return com3 != nullptr;
        case Serial::COM4 :
            return com4 != nullptr;
        default:
            return false;
    }
}
