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
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#ifndef HHUOS_LIB_LIBC_TIME
#define HHUOS_LIB_LIBC_TIME

#include <stddef.h>

struct tm {
	int tm_sec; // Seconds after minutes 0-61
	int tm_min; // 0-59
	int tm_hour; // 0-23
	int tm_mday; // 1-31
	int tm_mon; // 0-11
	int tm_year; // Year since 1900
	int tm_wday; // 0-6 weekday starting at sunday
	int tm_yday; // 0-365 days since jan 1
	int tm_isdst; // 1 = daylight saving time, 0 = no, -1 = no information
};

typedef long int time_t; // UNIX time
typedef long int clock_t; 

#define CLOCKS_PER_SEC 1000

#ifdef __cplusplus
extern "C" {
#endif

void libc_initialize_time(); // Called by runtime on application start

// Time manipulation
double difftime(time_t time_end, time_t time_beg);
time_t time(time_t *arg);
clock_t clock();

// Format conversion
char* asctime(const struct tm* time_ptr);
char* ctime(const time_t* timer);
size_t strftime(char* str, size_t count, const char* format, const struct tm* tp);
struct tm* gmtime( const time_t* timer);
struct tm* localtime( const time_t* timer);
time_t mktime(struct tm* arg);

#ifdef __cplusplus
}
#endif


#endif