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

#ifndef __Date_include__
#define __Date_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include <kernel/services/TimeService.h>
#include "Command.h"

/**
 * Implementation of Command.
 * Shows the system's Date.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Date : public Command {

private:
    TimeService *timeService = nullptr;

    static const char *weekdays[7];
    static const char *months[12];

    /**
     * Calculate the weekday of a specified date.
     *
     * Based on: https://blog.artofmemory.com/how-to-calculate-the-day-of-the-week-4203.html
     *
     * @param date The date
     *
     * @return The calculate weekday (0-6)
     */
    uint8_t calculateDayOfWeek(Rtc::Date date);

public:
    /**
     * Default-constructor.
     */
    Date() = delete;

    /**
     * Copy-constructor.
     */
    Date(const Date &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Date(Shell &shell);

    /**
     * Destructor.
     */
    ~Date() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};

#endif
