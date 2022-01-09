/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/Exception.h"
#include "TimeService.h"

namespace Kernel {

TimeService::TimeService(Device::TimeProvider *timeProvider, Device::DateProvider *dateProvider) : timeProvider(timeProvider), dateProvider(dateProvider) {}

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

TimeService::~TimeService() {
    delete timeProvider;
    delete dateProvider;
}

}