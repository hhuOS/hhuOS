/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include <stdarg.h>

#include "lib/util/base/Panic.h"
#include "TimeService.h"
#include "device/time/DateProvider.h"
#include "device/time/TimeProvider.h"
#include "lib/util/base/System.h"
#include "InterruptService.h"
#include "kernel/service/Service.h"
#include "device/time/WaitTimer.h"

namespace Kernel {

TimeService::TimeService(Device::WaitTimer *waitTimer) : waitTimer(waitTimer) {
    ASSIGN_SYSTEM_CALL(Util::System::GET_SYSTEM_TIME, TimeService::systemCallGetSystemTime);
    ASSIGN_SYSTEM_CALL(Util::System::GET_CURRENT_DATE, TimeService::systemCallGetCurrentDate);
    ASSIGN_SYSTEM_CALL(Util::System::SET_DATE, TimeService::systemCallSetDate);
}

void TimeService::setTimeProvider(Device::TimeProvider *timeProvider) {
    delete TimeService::timeProvider;
    TimeService::timeProvider = timeProvider;
}

void TimeService::setDateProvider(Device::DateProvider *dateProvider) {
    delete TimeService::dateProvider;
    TimeService::dateProvider = dateProvider;
}

Util::Time::Timestamp TimeService::getSystemTime() const {
    if (timeProvider == nullptr) {
        return Util::Time::Timestamp::ofSeconds(0);
    }

    return timeProvider->getTime();
}

Util::Time::Date TimeService::getCurrentDate() const {
    if (dateProvider != nullptr) {
        return dateProvider->getCurrentDate();
    }

    return Util::Time::Date(0);
}

bool TimeService::setCurrentDate(const Util::Time::Date &date) const {
    if (dateProvider != nullptr) {
        dateProvider->setCurrentDate(date);
        return true;
    }

    return false;
}

void TimeService::busyWait(const Util::Time::Timestamp &time) const {
    waitTimer->wait(time);
}

int64_t TimeService::systemCallGetSystemTime() {
    const auto &timeService = getService<TimeService>();

    return timeService.getSystemTime().toNanoseconds();
}

int64_t TimeService::systemCallGetCurrentDate() {
    const auto &timeService = getService<TimeService>();

    return timeService.getCurrentDate().getUnixTime();
}

int64_t TimeService::systemCallSetDate(const uint32_t unixTimeLow, const uint32_t unixTimeHigh) {
    auto &timeService = getService<TimeService>();
    const auto unixTime = static_cast<int64_t>(unixTimeLow) | (static_cast<int64_t>(unixTimeHigh) << 32);
    const auto date = Util::Time::Date(unixTime);

    timeService.setCurrentDate(date);
    return 0;
}

}
