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
#include "Date.h"
#include "kernel/process/ProcessScheduler.h"
#include "kernel/process/Process.h"


const char *Date::weekdays[7] = {
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

const char *Date::months[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"
};

Date::Date(Shell &shell) : Command(shell) {

};

void Date::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("pretty-print", "p");
    parser.addParameter("set", "s");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    timeService = Kernel::System::getService<Kernel::TimeService>();

    Rtc::Date date = timeService->getCurrentDate();

    String dateString = parser.getNamedArgument("set");

    if(!dateString.isEmpty()) {
        Util::Array<String> tmp = dateString.split("-");

        if(tmp.length() != 2) {
            stderr << "Invalid format! Please use dd.mm.(yy)yy-hh:mm:ss!" << endl;
            return;
        }

        Util::Array<String> date = tmp[0].split(".");
        Util::Array<String> time = tmp[1].split(":");

        if(date.length() != 3 || time.length() != 3) {
            stderr << "Invalid format! Please use dd.mm.(yy)yy-hh:mm:ss!" << endl;
            return;
        }

        Rtc::Date newDate;
        newDate.dayOfMonth = strtoint((const char*) date[0]);
        newDate.month = strtoint((const char*) date[1]);
        newDate.year = strtoint((const char*) date[2]);
        newDate.hours = strtoint((const char*) time[0]);
        newDate.minutes = strtoint((const char*) time[1]);
        newDate.seconds = strtoint((const char*) time[2]);

        timeService->getRTC()->setHardwareDate(newDate);

        return;
    }

    if(parser.checkSwitch("pretty-print")) {
        printf("  %s %d. %s %02d:%02d:%02d %04d\n", weekdays[calculateDayOfWeek(date)], date.dayOfMonth,
               months[date.month-1], date.hours, date.minutes, date.seconds, date.year);
    } else {
        String processData = Kernel::ProcessScheduler::getInstance().getAllProcesses();

        printf("  %02d.%02d.%04d %02d:%02d:%02d\n", date.dayOfMonth, date.month, date.year, date.hours, date.minutes,
                date.seconds);
        printf("-----------------------\n");
        printf("Length : %d\n",Kernel::ProcessScheduler::getInstance().getLength());
        printf("Process info :\n %s\n",processData);        
    }
}

uint8_t Date::calculateDayOfWeek(Rtc::Date date) {
    const uint8_t monthCodes[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
    const uint8_t centuryCodes[4] = { 6, 0, 2, 4 };

    auto yearCode = (date.year % 1000 + ((date.year % 1000) / 4)) % 7;
    auto monthCode = monthCodes[date.month - 1];
    auto centuryCode = centuryCodes[(date.year / 100) % 4];
    auto leapYearCode = date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0) ? 1 : 0;

    auto ret = (yearCode + monthCode + centuryCode + date.dayOfMonth - leapYearCode ) % 7;

    // ret = ret == 0 ? 6 : ret - 1;
    
    return static_cast<uint8_t>(ret);
}

const String Date::getHelpText() {
    return "Shows the current date.\n\n"
           "Usage: date [OPTION]...\n\n"
           "Options:\n"
           "  -p, --pretty-print: Pretty print the current date.\n"
           "  -s, --set: Set the date (format: dd.mm.(yy)yy-hh:mm:ss).\n"
           "  -h, --help: Show this help-message.";
}

