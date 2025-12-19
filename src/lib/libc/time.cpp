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
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include "time.h"

#include "string.h"
#include "util/time/Timestamp.h"
#include "util/time/Date.h"
#include "util/io/stream/ByteArrayOutputStream.h"
#include "util/io/stream/PrintStream.h"

clock_t startTime;

// Called on program start
void libc_initialize_time() {
	startTime = static_cast<clock_t>(Util::Time::Timestamp::getSystemTime().toMilliseconds());
}

// Time manipulation
double difftime(const time_t time_end, const time_t time_beg) {
	return time_end - time_beg;
}

time_t time(time_t *arg) {
	const auto ret = static_cast<time_t>(Util::Time::Date().getUnixTime());
	if (arg) {
		*arg = ret;
	}

	return ret;
}

clock_t clock() {
	return static_cast<clock_t>(Util::Time::Timestamp::getSystemTime().toMilliseconds()) - startTime;
}

// Format conversion
tm retTm; // tm struct to be used for conversions

void date_to_tm(const Util::Time::Date date, tm* tm_struct) {
	tm_struct->tm_sec = date.getSeconds();
	tm_struct->tm_min = date.getMinutes();
	tm_struct->tm_hour = date.getHours();
	tm_struct->tm_mday = date.getDayOfMonth();
	tm_struct->tm_mon = date.getMonth() - 1;
	tm_struct->tm_year = date.getYear() - 1900;
	
	// Conversion from Mon = 0 to Sun = 0;
	tm_struct->tm_wday = date.getWeekday();
	if (tm_struct->tm_wday == 6) {
		tm_struct->tm_wday = 0;
	} else {
		tm_struct->tm_wday++;
	}
	
	tm_struct->tm_yday = date.getDayOfYear() - 1;
	tm_struct->tm_isdst = 0; // Timezones unimplemented
}

Util::Time::Date tm_to_date(const tm* arg) {
	return Util::Time::Date(arg->tm_sec, arg->tm_min, arg->tm_hour,
		arg->tm_mday, arg->tm_mon + 1, static_cast<int16_t>(arg->tm_year + 1900));
}

tm* gmtime(const time_t* timer) {
	const auto date = Util::Time::Date(*timer);
	date_to_tm(date, &retTm);
	return &retTm;
}

tm* localtime(const time_t* timer) {
	return gmtime(timer); // Timezones unimplemented
}

time_t mktime(tm* arg) {
	const auto date = tm_to_date(arg);
	const auto ret = static_cast<time_t>(date.getUnixTime());

	date_to_tm(date, arg);
	return ret;
}

Util::Io::ByteArrayOutputStream timeBuf;

char* asctime(const tm* time_ptr) {
	timeBuf.reset();

	Util::String::formatDate(tm_to_date(time_ptr), timeBuf);
	timeBuf.write('\n');
	timeBuf.write('\0');

	return reinterpret_cast<char*>(timeBuf.getBuffer());
}


char* ctime(const time_t* timer) {
	return asctime(localtime(timer));
}


size_t strftime(char* str, const size_t count, const char* format, const tm* tp) {
	Util::Io::ByteArrayOutputStream byteBuf(reinterpret_cast<uint8_t*>(str), count);

	const auto written = Util::String::formatDate(tm_to_date(tp), byteBuf, format);
	if (byteBuf.write('\0')) {
		return written;
	}

	return 0;
}
