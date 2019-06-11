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

#include "lib/libc/printf.h"
#include "Uptime.h"

Uptime::Uptime(Shell &shell) : Command(shell) {

};

void Uptime::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);
    parser.addSwitch("pretty", "p");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    bool pretty = parser.checkSwitch("pretty");

    auto *timeService = Kernel::System::getService<Kernel::TimeService>();

    uint32_t millis = timeService->getSystemTime();

    auto days = millis / 1000 / 60 / 60 / 24;
    millis -= days * 24 * 60 * 60 * 1000;

    auto hours = millis / 1000 / 60 / 60;
    millis -= hours * 60 * 60 * 1000;

    auto minutes = millis / 1000 / 60;
    millis -= minutes * 60 * 1000;

    auto seconds = millis / 1000;

    if(pretty) {
        stdout << "  up ";

        if (days > 0) {
            stdout << dec << days << " day";

            if (days != 1) {
                stdout << "s";
            }

            stdout << ", ";
        }

        if (hours > 0) {
            stdout << dec << hours << " hour";

            if (hours != 1) {
                stdout << "s";
            }

            stdout << ", ";
        }

        if (minutes > 0) {
            stdout << dec << minutes << " minute";

            if (minutes != 1) {
                stdout << "s";
            }

            stdout << ", ";
        }

        stdout << dec << seconds << " second";

        if (seconds != 1) {
            stdout << "s";
        }

        stdout << endl;
    } else {
        if (days > 0) {
            stdout << dec << days << " day";

            if (days != 1) {
                stdout << "s";
            }

            stdout << " ";
        }

        printf("  %02d:%02d:%02d\n", hours, minutes, seconds);
    }
}

const String Uptime::getHelpText() {
    return "Shows the system's uptime.\n"
           "Usage: uptime [OPTION]...\n"
           "Options:\n"
           "  -p, --pretty: Show uptime in pretty format.\n"
           "  -h, --help: Show this help-message.";
}
