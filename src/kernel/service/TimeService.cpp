/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include <cstdarg>

#include "lib/util/base/Exception.h"
#include "TimeService.h"
#include "device/time/DateProvider.h"
#include "device/time/TimeProvider.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/base/System.h"
#include "InterruptService.h"
#include "kernel/service/Service.h"

namespace Device {
class Rtc;
}  // namespace Device

namespace Kernel {

TimeService::TimeService(Device::TimeProvider *timeProvider, Device::DateProvider *dateProvider) : timeProvider(timeProvider), dateProvider(dateProvider) {
    Service::getService<InterruptService>().assignSystemCall(Util::System::GET_SYSTEM_TIME, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &timeService = Service::getService<TimeService>();
        auto &targetTime = *va_arg(arguments, Util::Time::Timestamp*);

        targetTime = timeService.getSystemTime();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::GET_CURRENT_DATE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &timeService = Service::getService<TimeService>();
        auto &targetDate = *va_arg(arguments, Util::Time::Date*);

        targetDate = timeService.getCurrentDate();
        return true;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::SET_DATE, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &timeService = Service::getService<TimeService>();
        auto &date = *va_arg(arguments, Util::Time::Date*);

        timeService.setCurrentDate(date);
        return true;
    });
}

TimeService::~TimeService() {
    delete timeProvider;
    delete dateProvider;
}

Util::Time::Timestamp TimeService::getSystemTime() const {
    if (timeProvider != nullptr) {
        return timeProvider->getTime();
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "TimeService: No time provider available!");
}

Util::Time::Date TimeService::getCurrentDate() const {
    if (dateProvider != nullptr) {
        return dateProvider->getCurrentDate();
    }

    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "TimeService: No date provider available!");
}

void TimeService::setCurrentDate(const Util::Time::Date &date) {
    if (dateProvider != nullptr) {
        return dateProvider->setCurrentDate(date);
    } else {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "TimeService: No date provider available!");
    }
}

void TimeService::busyWait(const Util::Time::Timestamp &time) const {
    auto end = getSystemTime().toMilliseconds() + time.toMilliseconds();

    while (getSystemTime().toMilliseconds() < end) {
        Service::getService<SchedulerService>().yield();
    }
}

Device::Rtc *TimeService::getRtc() {
    return reinterpret_cast<Device::Rtc *>(dateProvider);
}

}