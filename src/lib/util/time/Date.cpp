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

#include "Date.h"

#include "lib/interface.h"

namespace Util::Time {

Date::Date() : Date(getCurrentDate()) {}

Date::Date(const uint8_t seconds, const uint8_t minutes, const uint8_t hours,
           const uint8_t dayOfMonth, const uint8_t month, const int16_t year) :
	seconds(seconds), minutes(minutes), hours(hours),
	dayOfMonth(dayOfMonth), month(month), year(year) {}

Date::Date(int64_t unixTime) {
	if (unixTime < 0) {
		Panic::fire(Panic::UNSUPPORTED_OPERATION, "Date: Unix time before 1970 is not supported.");
	}

	year = 1970;
	while (unixTime >= getLengthOfYear(year) * secondsPerDay) {
		unixTime -= getLengthOfYear(year) * secondsPerDay;
		year++;
	}

	month = 1;
	while (unixTime >= getLengthOfMonth(month, year) * secondsPerDay) {
		unixTime -= getLengthOfMonth(month, year) * secondsPerDay;
		month ++;
	}

	dayOfMonth = 1;
	while (unixTime >= secondsPerDay) {
		unixTime -= secondsPerDay;
		dayOfMonth++;
	}

	hours = 0;
	while (unixTime >= 3600) {
		unixTime -= 3600;
		hours++;
	}

	minutes = 0;
	while (unixTime >= 60) {
		unixTime -= 60;
		minutes++;
	}

	seconds = unixTime;
}


bool Date::operator==(const Date &other) const {
	return seconds == other.seconds && minutes == other.minutes && hours == other.hours &&
		dayOfMonth == other.dayOfMonth && month == other.month && year == other.year;
}

bool Date::operator!=(const Date &other) const {
	return seconds != other.seconds || minutes != other.minutes || hours != other.hours ||
		dayOfMonth != other.dayOfMonth || month != other.month || year != other.year;
}

uint8_t Date::getSeconds() const {
    return seconds;
}

uint8_t Date::getMinutes() const {
    return minutes;
}

uint8_t Date::getHours() const {
    return hours;
}

uint8_t Date::getDayOfMonth() const {
    return dayOfMonth;
}

uint8_t Date::getMonth() const {
    return month;
}

int16_t Date::getYear() const {
    return year;
}

int64_t Date::getUnixTime() const {
	if (year < 1970) {
		Panic::fire(Panic::UNSUPPORTED_OPERATION, "Date: Unix time before 1970 is not supported.");
	}

	int64_t ret = 0;
	for (int16_t i = 1970; i < year; i++) {
		ret += getLengthOfYear(i) * secondsPerDay;
	}

	for (uint8_t i = 1; i < month; i++) {
		ret += getLengthOfMonth(i, year) * secondsPerDay;
	}

	ret += (dayOfMonth - 1) * secondsPerDay;
	ret += hours * 3600;
	ret += minutes * 60;
	ret += seconds;
	
	return ret;
}

uint16_t Date::getDayOfYear() const {
	uint16_t ret = 0;
	for (uint8_t i = 1; i < month; i++) {
		ret += getLengthOfMonth(i, year);
	}

	return ret + dayOfMonth;
}

uint8_t Date::getWeekday() const {
	return static_cast<uint8_t>((3 + getUnixTime() / secondsPerDay) % 7); // 1.1.1970 was a Thursday
}

uint8_t Date::getWeekOfYear() const {
	const auto firstWeekday = Date(0, 0, 0, 1, 1, year).getWeekday();
	return (getDayOfYear() - 1 + firstWeekday) / 7 + 1;
}

uint8_t Date::getWeekOfYearSunday() const {
	auto firstWeekday = Date(0, 0, 0, 1, 1, year).getWeekday();
	firstWeekday = firstWeekday == 6 ? 0 : firstWeekday + 1;

	return (getDayOfYear() - 1 + firstWeekday) / 7 + 1;
}

void Date::setHardwareClock() const {
    ::setDate(*this);
}

uint16_t Date::getLengthOfYear(const int16_t year) {
	if (year % 4 == 0) {
		if (year % 100 == 0) {
			if (year % 400 == 0) {
				return 366;
			}
			return 365;
		}
		return 366;
	}
	return 365;
}

uint8_t Date::getLengthOfMonth(const uint8_t month, const int16_t year) {
	if (month == 2) { // February -> Check for leap year
		return getLengthOfYear(year) == 366 ? 29 : 28;
	}

	return monthLength[month - 1];
}

}