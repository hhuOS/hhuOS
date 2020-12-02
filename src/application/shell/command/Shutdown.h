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

#ifndef __Shutdown_include__
#define __Shutdown_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
// #include "kernel/service/TimeService.h"
#include "Command.h"
#include "device/cpu/IoPort.h"

class Shutdown : public Command {

private:
    // Kernel::TimeService *timeService = nullptr;

    // static const char *weekdays[7];
    // static const char *months[12];

    /**
     * Calculate the weekday of a specified date.
     *
     * Based on: https://blog.artofmemory.com/how-to-calculate-the-day-of-the-week-4203.html
     *
     * @param date The date
     *
     * @return The calculate weekday (0-6)
     */
    // uint8_t calculateDayOfWeek(Rtc::Date date);

    // const IoPort dataPort;

public:
    /**
     * Default-constructor.
     */
    Shutdown() = delete;

    /**
     * Copy-constructor.
     */
    // Shutdown(const Date &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Shutdown(Shell &shell);

    /**
     * Destructor.
     */
    ~Shutdown() override = default;

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
