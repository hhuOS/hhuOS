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

#include <lib/libc/printf.h>
#include "Date.h"

const char *Date::weekdays[7] = {
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

const char *Date::months[12] = {
        "Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

Date::Date(Shell &shell) : Command(shell) {

};

void Date::execute(Util::Array<String> &args) {
    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Shows the system's Date." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    timeService = Kernel::getService<TimeService>();

    Rtc::Date date = timeService->getCurrentDate();

    printf("%s %d. %s %02d:%02d:%02d %04d\n", weekdays[calculateDayOfWeek(date)], date.dayOfMonth, months[date.month],
           date.hours, date.minutes, date.seconds, date.year);
}

uint8_t Date::calculateDayOfWeek(Rtc::Date date) {
    const uint8_t monthCodes[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
    const uint8_t centuryCodes[4] = { 6, 0, 2, 4 };

    auto yearCode = (date.year % 1000 + ((date.year % 1000) / 4)) % 7;
    auto monthCode = monthCodes[date.month - 1];
    auto centuryCode = centuryCodes[(date.year / 100) % 4];
    auto leapYearCode = date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0) ? 1 : 0;

    auto ret = (yearCode + monthCode + centuryCode + date.dayOfMonth - leapYearCode) % 7;

    ret = ret == 0 ? 6 : ret - 1;

    return static_cast<uint8_t>(ret);
}
