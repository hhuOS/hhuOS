/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/Exception.h"
#include "TimeService.h"
#include "kernel/system/SystemCall.h"
#include "kernel/system/System.h"
#include "device/time/DateProvider.h"
#include "device/time/TimeProvider.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/system/System.h"

namespace Kernel {

TimeService::TimeService(Device::TimeProvider *timeProvider, Device::DateProvider *dateProvider) : timeProvider(timeProvider), dateProvider(dateProvider) {
    SystemCall::registerSystemCall(Util::System::GET_SYSTEM_TIME, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        Util::Time::Timestamp *targetTime = va_arg(arguments, Util::Time::Timestamp*);

        *targetTime = System::getService<TimeService>().getSystemTime();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_DATE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        Util::Time::Date *targetDate = va_arg(arguments, Util::Time::Date*);

        *targetDate = System::getService<TimeService>().getCurrentDate();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::SET_DATE, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        if (paramCount < 1) {
            return Util::System::INVALID_ARGUMENT;
        }

        Util::Time::Date *date = va_arg(arguments, Util::Time::Date*);

        System::getService<TimeService>().setCurrentDate(*date);
        return Util::System::Result::OK;
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
        System::getService<SchedulerService>().yield();
    }
}

}