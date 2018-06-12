/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kernel/services/SerialService.h>
#include "ComConfig.h"

ComConfig::ComConfig(Shell &shell) : Command(shell) {

};

void ComConfig::execute(Util::Array<String> &args) {
    uint8_t port = 0;
    uint32_t baud = 0;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-h" || args[i] == "--help") {
            stdout << "ComConfigs the screen." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            stdout << "  -p, --port: Set the port (1-4)." << endl;
            stdout << "  -b, --baud: Set the baud-rate." << endl;
            return;
        } else if(args[i] == "-p" || args[i] == "--port") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            }

            port = static_cast<uint8_t>(strtoint((const char *) args[++i]));
        } else if(args[i] == "-b" || args[i] == "--baud") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            }

            baud = static_cast<uint32_t>(strtoint((const char *) args[++i]));
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(port < 1 || port > 4) {
        stderr << args[0] << ": Please enter valid port (1-4) using '--port'!" << endl;
        return;
    }

    if(!Kernel::getService<SerialService>()->isPortAvailable(static_cast<Serial::ComPort>(port))) {
        stderr << args[0] << ": COM" << static_cast<uint32_t>(port) << " is not available on this machine!" << endl;
        return;
    }

    Serial *serial = Kernel::getService<SerialService>()->getSerialPort(static_cast<Serial::ComPort>(port));

    if(baud != 0) {
        if (115200 % baud != 0 || baud < 2) {
            stderr << args[0] << ": Invalid baud rate '" << baud
                   << "'! The baud rate must be a restless divider of 115200 greater than 1!" << endl;
            return;
        }

        if (baud == 115200) {
            serial->setSpeed(Serial::BaudRate::BAUD_115200);
        } else {
            serial->setSpeed(static_cast<Serial::BaudRate>(115200 / baud));
        }
    }

    stdout << "COM" << static_cast<uint32_t>(port) << ":" << endl;
    stdout << "  Speed: " << 115200 / static_cast<uint32_t>(serial->getSpeed()) << " Baud" << endl;
}