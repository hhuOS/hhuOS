#ifndef LIBC_TIME
#define LIBC_TIME

#include <stddef.h>

struct tm {
	int tm_sec; //seconds after minutes 0-61
	int tm_min; //0-59
	int tm_hour; //0-23
	int tm_mday; //1-31
	int tm_mon; //0-11
	int tm_year; //year since 1900
	int tm_wday; //0-6 weekday starting at sunday
	int tm_yday; //0-365 days since jan 1
	int tm_isdst; //1 = daylight saving time, 0 = no, -1 = no information
};

typedef long int time_t; //UNIX time
typedef long int clock_t; 

#define CLOCKS_PER_SEC 1000

#ifdef __cplusplus
extern "C" {
#endif

void _time_initialize();

//time manipulation
double difftime(time_t time_end, time_t time_beg);
time_t time(time_t *arg);
clock_t clock();

//format conversion
char * asctime(const struct tm* time_ptr);
char * ctime(const time_t* timer);
size_t strftime(char* str, size_t count, const char* format, const struct tm* tp);
struct tm* gmtime( const time_t* timer);
struct tm* localtime( const time_t* timer);
time_t mktime(struct tm* arg);



#ifdef __cplusplus
}
#endif


#endif