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

#include "Date.h"
#include "lib/interface.h"

namespace Util::Time {

Date::Date(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayOfMonth, uint8_t month, uint16_t year) :
        seconds(seconds), minutes(minutes), hours(hours), dayOfMonth(dayOfMonth), month(month), year(year) {}



uint16_t getLengthOfYear(int16_t year) {
	if (year % 4 == 0) {
		if (year % 100 == 0) {
			if (year % 400 == 0) return 366;
			return 365;
		}
		return 366;
	}
	return 365;
}

const uint8_t _monthLength[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
uint8_t getLengthOfMonth(uint8_t month, int16_t year) {
	if (month == 2) return getLengthOfYear(year) == 366 ? 29 : 28;
	return _monthLength[month-1];
}

const int secPerDay = 86400;

Date::Date(int64_t unixTime) {
	if (unixTime >0) {
		Date::year = 1970;
		while (unixTime >= getLengthOfYear(Date::year) * secPerDay) {
			unixTime -= getLengthOfYear(Date::year) * secPerDay;
			Date::year++;
		}
		
		Date::month = 1;
		while (unixTime >= getLengthOfMonth(Date::month, Date::year) * secPerDay) {
			unixTime -= getLengthOfMonth(Date::month, Date::year) * secPerDay;
			Date::month ++;
		}
		
		Date::dayOfMonth = 1;
		while (unixTime >= secPerDay) {
			unixTime -= secPerDay;
			Date::dayOfMonth++;
		}
		
		Date::hours = 0;
		while (unixTime >= 3600) {
			unixTime -= 3600;
			Date::hours++;
		}
		
		Date::minutes = 0;
		while (unixTime >= 60) {
			unixTime -= 60;
			Date::minutes++;
		}
		
		Date::seconds = unixTime;
	} else {
		//Time before 1970 currently unimplemented
		Date::year = 1970;
		Date::dayOfMonth = 1;
		Date::month = 1;
		Date::hours = 0;
		Date::minutes = 0;
		Date::seconds = 0;
	}
}


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


int64_t Date::getUnixTime() {
	int64_t ret = 0;
	if (Date::year < 1970) return 0; //Years before 1970 unimplemented
	
	for (int i=1970; i<Date::year; i++) ret += getLengthOfYear(i) * secPerDay;
	for (int i=1; i<Date::month; i++) ret += getLengthOfMonth(i, Date::year) * secPerDay;
	ret += (Date::dayOfMonth - 1) * secPerDay;
	ret += Date::hours * 3600;
	ret += Date::minutes * 60;
	ret += Date::seconds;
	
	return ret;
	
}

uint16_t Date::getDayOfYear() {
	uint16_t ret = 0;
	for (int i=0; i<Date::month; i++) ret += getLengthOfMonth(i, Date::year);
	return ret + Date::dayOfMonth;
}

int8_t Date::getWeekday() {
	return (3 + (((int32_t)this->getUnixTime()) / secPerDay)) % 7; //1.1.1970 was a Thursday
}

uint8_t Date::getWeekOfYear() {
	auto firstWeekday = Date(0,0,0,1,1,Date::year).getWeekday();
	return (getDayOfYear() + firstWeekday) / 7;
}

uint8_t Date::getWeekOfYearSunday() {
	auto firstWeekday = Date(0,0,0,1,1,Date::year).getWeekday();
	if (firstWeekday == 6) firstWeekday = 0;
	else firstWeekday++;
	return (getDayOfYear() + firstWeekday) / 7;
}
	


}