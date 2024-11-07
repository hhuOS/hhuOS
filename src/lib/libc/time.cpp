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


char * asctime(const struct tm* time_ptr) {
	timeBuf.reset();
	timeOut.print(_tm_to_date(time_ptr));
	timeBuf.write('\n');
	timeBuf.write('\0');
	return (char*)timeBuf.getBuffer();
}


char * ctime(const time_t* timer) {
	return asctime(localtime(timer));
}


size_t strftime(char* str, size_t count, const char* format, const struct tm* tp) {
	auto byteBuf = Util::Io::ByteArrayOutputStream((uint8_t*)str, count);
	byteBuf.setEnforceSizeLimit(true);
	
	auto printStr = Util::Io::PrintStream(byteBuf);
	
	printStr.printFormatted(format,  _tm_to_date(tp));
	printStr.flush();
	
	byteBuf.write('\0');
	
	if (byteBuf.sizeLimitReached()) return 0;
	
	return byteBuf.getLength() - 1;
}
