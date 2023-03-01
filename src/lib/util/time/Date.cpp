/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Date.h"
#include "lib/interface.h"

namespace Util::Time {

Date::Date(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayOfMonth, uint8_t month, uint16_t year) :
        seconds(seconds), minutes(minutes), hours(hours), dayOfMonth(dayOfMonth), month(month), year(year) {}

uint8_t Date::getSeconds() const {
    return seconds;
}

void Date::setSeconds(uint8_t seconds) {
    Date::seconds = seconds;
}

uint8_t Date::getMinutes() const {
    return minutes;
}

void Date::setMinutes(uint8_t minutes) {
    Date::minutes = minutes;
}

uint8_t Date::getHours() const {
    return hours;
}

void Date::setHours(uint8_t hours) {
    Date::hours = hours;
}

uint8_t Date::getDayOfMonth() const {
    return dayOfMonth;
}

void Date::setDayOfMonth(uint8_t dayOfMonth) {
    Date::dayOfMonth = dayOfMonth;
}

uint8_t Date::getMonth() const {
    return month;
}

void Date::setMonth(uint8_t month) {
    Date::month = month;
}

uint16_t Date::getYear() const {
    return year;
}

void Date::setYear(uint16_t year) {
    Date::year = year;
}

Date getCurrentDate() {
    return ::getCurrentDate();
}

void setDate(const Date &date) {
    ::setDate(date);
}

}