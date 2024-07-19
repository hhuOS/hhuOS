#include "lib/libc/time.h"

#include "lib/libc/string.h"

#include "lib/util/time/Timestamp.h"
#include "lib/util/time/Date.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/io/stream/PrintStream.h"


clock_t startTime;

//Called on program start
void _time_initialize() {
	startTime = Util::Time::getSystemTime().toMilliseconds();
}

//time manipulation
double difftime(time_t time_end, time_t time_beg) {
	return time_end - time_beg;
}

time_t time(time_t *arg) {
	time_t ret = Util::Time::getCurrentDate().getUnixTime();
	if (arg) *arg = ret;
	return ret;
}

clock_t clock() {
	return Util::Time::getSystemTime().toMilliseconds() - startTime;
}

//format conversion
struct tm retTm; //tm struct to be used for conversions

void _date_to_tm(Util::Time::Date date, struct tm* tm_struct) {
	tm_struct->tm_sec = date.getSeconds();
	tm_struct->tm_min = date.getMinutes();
	tm_struct->tm_hour = date.getHours();
	tm_struct->tm_mday = date.getDayOfMonth();
	tm_struct->tm_mon = date.getMonth();
	tm_struct->tm_year = date.getYear() - 1900;
	
	//Conversion from Mon=0 to Sun=0;
	tm_struct->tm_wday = date.getWeekday();
	if (tm_struct->tm_wday == 6) tm_struct->tm_wday = 0;
	else tm_struct->tm_wday++;
	
	tm_struct->tm_yday = date.getDayOfYear();
	tm_struct->tm_isdst = 0; //timezones unimplemented 
}

Util::Time::Date _tm_to_date( const struct tm* arg) {
	return Util::Time::Date(arg->tm_sec, arg->tm_min, arg->tm_hour, arg->tm_mday, arg->tm_mon, arg->tm_year + 1900);
}

struct tm* gmtime( const time_t* timer) {
	Util::Time::Date date = Util::Time::Date(*timer);
	_date_to_tm(date, &retTm);
	return &retTm;
}

struct tm* localtime( const time_t* timer) {
	return gmtime(timer); //timezones unimplemented
}

time_t mktime(struct tm* arg) {
	auto date = _tm_to_date(arg);
	time_t ret = date.getUnixTime();
	_date_to_tm(date, arg);
	return ret;
}


Util::Io::ByteArrayOutputStream timeBuf(1024);
Util::Io::PrintStream timeOut = Util::Io::PrintStream(timeBuf);

const char * weekdayAbbreviations[7] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

const char * weekdayNames[7] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

const char * monthAbbreviations[13] = {
	"",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

const char * monthNames[13] = {
	"",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};


char * _asctime(const struct tm* time_ptr, bool includeNewline) {
	timeBuf.reset();
	timeOut << weekdayAbbreviations[time_ptr->tm_wday] << " ";
	timeOut << monthAbbreviations[time_ptr->tm_mon]<<" ";
	timeOut.setIntegerPrecision(2);
	timeOut << time_ptr->tm_mday<<" ";
	timeOut << time_ptr->tm_hour<<":"<<time_ptr->tm_min<<":"<<time_ptr->tm_sec;
	timeOut.setIntegerPrecision(4);
	timeOut << " " << time_ptr->tm_year + 1900;
	timeOut.flush();
	if (includeNewline) timeBuf.write('\n');
	timeBuf.write('\0');
	return (char*)timeBuf.getBuffer();
}


char * asctime(const struct tm* time_ptr) {
	return _asctime(time_ptr, true);
}


char * ctime(const time_t* timer) {
	return asctime(localtime(timer));
}


size_t strftime(char* str, size_t count, const char* format, const struct tm* tp) {
	auto byteBuf = Util::Io::ByteArrayOutputStream((uint8_t*)str, count);
	byteBuf.setEnforceSizeLimit(true);
	
	auto printStr = Util::Io::PrintStream(byteBuf);
	
	for (; *format; format++) {
		if (*format != '%') {
			printStr << *format;
		} else {
			format++;
			switch (*format) {
				case '%':
					printStr << '%';
					break;
					
				case 'Y':
					printStr << tp->tm_year + 1900;
					break;
				
				case 'y':
					printStr << (tp->tm_year)%100;
					break;
				
				case 'b':
					printStr << monthAbbreviations[tp->tm_mon];
					break;
					
				case 'B':
					printStr << monthNames[tp->tm_mon];
					break;
					
				case 'm':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_mon;
					printStr.setIntegerPrecision(-1);
					break;
					
				case 'U':
					printStr.setIntegerPrecision(2);
					printStr << _tm_to_date(tp).getWeekOfYearSunday();
					printStr.setIntegerPrecision(0);
					break;
					
				case 'W':
					printStr.setIntegerPrecision(2);
					printStr << _tm_to_date(tp).getWeekOfYear();
					printStr.setIntegerPrecision(0);
					break;
					
				case 'j':
					printStr.setIntegerPrecision(3);
					printStr << tp->tm_yday;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'd':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_mday;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'a':
					printStr << weekdayAbbreviations[tp->tm_wday];
					break;
					
				case 'A':
					printStr << weekdayNames[tp->tm_wday];
					break;
					
				case 'w':
					printStr << tp->tm_wday;
					break;
				
				case 'H':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_hour;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'I':
					printStr.setIntegerPrecision(2);
					printStr << ((tp->tm_hour - 1) % 12) + 1;
					printStr.setIntegerPrecision(0);
					break;
				
				case 'M':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_min;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'S':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_sec;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'c':
					printStr << _asctime(tp, false);
					break;
				
				case 'x':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_mday << "." << tp->tm_mon << "." << tp->tm_year+1900;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'X':
					printStr.setIntegerPrecision(2);
					printStr << tp->tm_hour << ":" << tp->tm_min << ":" << tp->tm_sec;
					printStr.setIntegerPrecision(0);
					break;
					
				case 'p':
					if (tp->tm_hour >= 13) printStr << "p.m.";
					else printStr << "a.m.";
					break;
					
				case 'Z':
					printStr << "UTC+0";
					break;
					
				default:
					break;
			}
		}
	}
	
	
	
	byteBuf.write('\0');
	
	if (byteBuf.sizeLimitReached()) return 0;
	
	return byteBuf.getLength() - 1;
}
