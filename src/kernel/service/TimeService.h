/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
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

#include "device/time/TimeProvider.h"
#include "device/time/DateProvider.h"
#include "Service.h"

namespace Kernel {

class TimeService : public Service {

public:
    /**
     * Constructor.
     */
    TimeService(Device::TimeProvider *timeProvider, Device::DateProvider *dateProvider);

    /**
     * Copy-constructor.
     */
    TimeService(const TimeService &copy) = delete;

    /**
     * Assignment operator.
     */
    TimeService& operator=(const TimeService &other) = delete;

    /**
     * Destructor.
     */
    ~TimeService() override;

    [[nodiscard]] Util::Time::Timestamp getSystemTime() const;

    [[nodiscard]] Util::Time::Date getCurrentDate() const;

    void setCurrentDate(const Util::Time::Date &date);

    static const constexpr uint8_t SERVICE_ID = 4;

private:

    Device::TimeProvider *timeProvider;
    Device::DateProvider *dateProvider;
};

}

#endif
