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

#ifndef HHUOS_LIB_UTIL_TIME_DATE_H
#define HHUOS_LIB_UTIL_TIME_DATE_H

#include <stdint.h>

namespace Util {
namespace Time {

/// Represents a date and time in the Gregorian calendar.
class Date {

public:
	/// Create a new Date instance with the current date and time.
	Date();

	/// Create a new Date instance with the specified values.
	Date(const uint8_t seconds, const uint8_t minutes, const uint8_t hours,
		const uint8_t dayOfMonth, const uint8_t month, const int16_t year) :
	seconds(seconds), minutes(minutes), hours(hours),
	dayOfMonth(dayOfMonth), month(month), year(year) {}

	/// Create a new Date instance from a Unix timestamp (seconds since 1. January 1970).
	/// Negative timestamps are not supported and will fire a panic.
	explicit Date(int64_t unixTime);

	/// Compare the Date with another one for equality.
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0); // Unix timestamp 0 -> 1. January 1970, 00:00:00
	/// const auto date2 = Util::Time::Date(0, 0, 0, 1, 1, 1970);
	/// const auto date3 = Util::Time::Date(0, 0, 0, 1, 1, 1971);
	///
	/// const auto equal1 = (date1 == date2); // true
	/// const auto equal2 = (date1 == date3); // false
	/// ```
	bool operator==(const Date &other) const {
		return seconds == other.seconds && minutes == other.minutes && hours == other.hours &&
			dayOfMonth == other.dayOfMonth && month == other.month && year == other.year;
	}

	/// Compare the Date with another one for inequality.
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0); // Unix timestamp 0 -> 1. January 1970, 00:00:00
	/// const auto date2 = Util::Time::Date(0, 0, 0, 1, 1, 1970);
	/// const auto date3 = Util::Time::Date(0, 0, 0, 1, 1, 1971);
	///
	/// const auto notEqual1 = (date1 != date2); // false
	/// const auto notEqual2 = (date1 != date3); // true
	/// ```
	bool operator!=(const Date &other) const {
		return seconds != other.seconds || minutes != other.minutes || hours != other.hours ||
			dayOfMonth != other.dayOfMonth || month != other.month || year != other.year;
	}

	/// Return the seconds (0-59) of the date.
	uint8_t getSeconds() const {
		return seconds;
	}

	/// Return the minutes (0-59) of the date.
	uint8_t getMinutes() const {
		return minutes;
	}

	/// Return the hours (0-23) of the date.
	uint8_t getHours() const {
		return hours;
	}

	/// Return the day of the month (1-31) of the date.
	uint8_t getDayOfMonth() const {
		return dayOfMonth;
	}

	/// Return the month (1-12) of the date.
	uint8_t getMonth() const {
		return month;
	}

	/// Return the year of the date.
	int16_t getYear() const {
		return year;
	}

	/// Convert the date to a Unix timestamp (seconds since 1. January 1970).
	/// Dates before 1. January 1970 are not supported and will fire a panic.
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 1970);
	/// const auto date2 = Util::Time::Date(0, 0, 0, 1, 1, 2000);
	/// const auto date3 = Util::Time::Date(0, 0, 0, 31, 12, 1969);
	///
	/// const auto unixTime1 = date1.getUnixTime(); // 0
	/// const auto unixTime2 = date2.getUnixTime(); // 946684800
	/// const auto unixTime3 = date3.getUnixTime(); // Panic! (date before 1970)
	/// ```
	int64_t getUnixTime() const;

	/// Calculate the day of the week (0 = Monday, 6 = Sunday).
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 1970); // 1. January 1970 was a Thursday
	/// const auto date2 = Util::Time::Date(0, 0, 0, 1, 1, 2000); // 1. January 2000 was a Saturday
	///
	/// const auto weekday1 = date1.getWeekday(); // 3 (Thursday)
	/// const auto weekday2 = date2.getWeekday(); // 5 (Saturday)
	/// ```
	uint8_t getWeekday() const;

	/// Calculate the day of the year (1-366).
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 1970); // 1. January 1970
	/// const auto date2 = Util::Time::Date(0, 0, 0, 31, 12, 2000); // 31. December 2000
	///
	/// const auto dayOfYear1 = date1.getDayOfYear(); // 1
	/// const auto dayOfYear2 = date2.getDayOfYear(); // 366 (2000 was a leap year)
	/// ```
	uint16_t getDayOfYear() const;

	/// Calculate the week of the year (1-54).
	/// This calculation uses Monday as the first day of the week.
	/// It is not ISO 8601 compliant, as it considers the first week of the year to be the week containing January 1st,
	/// instead of the week containing the first Thursday of the year. For example, the 1. January 2000 is in week 1,
	/// instead of week 53 of 1999 as per ISO 8601.
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 1970); // 1. January 1970
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 2000); // 1. January 2000
	/// const auto date3 = Util::Time::Date(0, 0, 0, 31, 12, 2000); // 31. December 2000
	///
	/// const auto weekOfYear1 = date1.getWeekOfYear(); // 1
	/// const auto weekOfYear2 = date2.getWeekOfYear(); // 1
	/// const auto weekOfYear3 = date3.getWeekOfYear(); // 53
	/// ```
	uint8_t getWeekOfYear() const;

	/// Calculate the week of the year (1-54) with Sunday as the first day of the week.
	/// This calculation is not ISO 8601 compliant, as it considers the first week of the year to be
	/// the week containing January 1st, instead of the week containing the first Thursday of the year.
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 1970); // 1. January 1970
	/// const auto date2 = Util::Time::Date(0, 0, 0, 1, 1, 2000); // 1. January 2000
	/// const auto date3 = Util::Time::Date(0, 0, 0, 31, 12, 2000); // 31. December 2000
	///
	/// const auto weekOfYearSunday1 = date1.getWeekOfYearSunday(); // 1
	/// const auto weekOfYearSunday2 = date2.getWeekOfYearSunday(); // 1
	/// const auto weekOfYearSunday3 = date3.getWeekOfYearSunday(); // 54 (31. December 2000 was a Sunday)
	/// ```
	uint8_t getWeekOfYearSunday() const;

	/// Set the hardware clock to this date.
	///
	/// ### Example
	/// ```c++
	/// const auto date1 = Util::Time::Date(0, 0, 0, 1, 1, 2000); // 1. January 2000
	/// date1.setHardwareClock(); // Sets the hardware clock to 1. January 2000, 00:00:00
	///
	/// const auto date2 = Util::Time::Date(); // Current date and time (should now be 1. January 2000, 00:00:00)
	/// ```
	void setHardwareClock() const;

private:

	/// Calculate the number of days in the specified, taking leap years into account.
	static uint16_t getLengthOfYear(int16_t year);

	/// Calculate the number of days in the specified month, taking leap years into account.
	static uint8_t getLengthOfMonth(uint8_t month, int16_t year);

	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;
	uint8_t dayOfMonth = 1;
	uint8_t month = 1;
	int16_t year = 1970;

	static constexpr uint32_t secondsPerDay = 86400;
	static constexpr uint8_t monthLength[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
};

}
}

#endif