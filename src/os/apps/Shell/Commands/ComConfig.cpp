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

    ArgumentParser parser(getHelpText(), 1);
    parser.addParameter("port", "p", true);
    parser.addParameter("baud", "b", false);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    port = static_cast<uint8_t>(strtoint((const char *) parser.getNamedArgument("port")));
    baud = static_cast<uint8_t>(strtoint((const char *) parser.getNamedArgument("baud")));

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

const String ComConfig::getHelpText() {
    return "ComConfigs the screen.\n\n"
           "Usage: comconfig [OPTION]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.\n"
           "  -p, --port: Set the port (1-4).\n"
           "  -b, --baud: Set the baud-rate.";
}
