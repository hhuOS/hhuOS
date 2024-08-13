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

#include <stdarg.h>

#include "lib/util/base/Exception.h"
#include "TimeService.h"
#include "device/time/DateProvider.h"
#include "device/time/TimeProvider.h"
#include "lib/util/base/System.h"
#include "InterruptService.h"
#include "kernel/service/Service.h"
#include "device/time/WaitTimer.h"

namespace Kernel {

TimeService::TimeService(Device::WaitTimer *waitTimer) : waitTimer(waitTimer) {
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
    waitTimer->wait(time);
}

}