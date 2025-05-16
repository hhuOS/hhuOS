/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_TIMESERVICE_H
#define HHUOS_TIMESERVICE_H

#include <stdint.h>

#include "Service.h"
#include "lib/util/time/Date.h"
#include "lib/util/time/Timestamp.h"

namespace Device {
class WaitTimer;
class DateProvider;
class TimeProvider;
}  // namespace Device

namespace Kernel {

class TimeService : public Service {

public:
    /**
     * Constructor.
     */
    explicit TimeService(Device::WaitTimer *waitTimer);

    /**
     * Copy Constructor.
     */
    TimeService(const TimeService &copy) = delete;

    /**
     * Assignment operator.
     */
    TimeService& operator=(const TimeService &other) = delete;

    /**
     * Destructor.
     */
    ~TimeService() override = default;

    void setTimeProvider(Device::TimeProvider *timeProvider);

    void setDateProvider(Device::DateProvider *dateProvider);

    [[nodiscard]] Util::Time::Timestamp getSystemTime() const;

    [[nodiscard]] Util::Time::Date getCurrentDate() const;

    void setCurrentDate(const Util::Time::Date &date);

    void busyWait(const Util::Time::Timestamp &time) const;

    static const constexpr uint8_t SERVICE_ID = 6;

private:

    Device::WaitTimer *waitTimer;
    Device::TimeProvider *timeProvider = nullptr;
    Device::DateProvider *dateProvider = nullptr;
};

}

#endif
