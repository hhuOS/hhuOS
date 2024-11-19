#include "PrintStream.h"
#include "lib/util/time/Date.h"

namespace Util::Io {


const char * weekdayAbbreviations[7] = {
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat",
        "Sun"
};

const char * weekdayNames[7] = {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"
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


void PrintStream::print(Util::Time::Date date) {
    *this << weekdayAbbreviations[date.getWeekday()] << " ";
    *this << monthAbbreviations[date.getMonth()]<<" ";
    this->setIntegerPrecision(2);
    *this  << date.getDayOfMonth()<<" ";
    *this  << date.getHours()<<":"<<date.getMinutes()<<":"<<date.getSeconds();
    this->setIntegerPrecision(4);
    *this  << " " << date.getYear();
    this->flush();
}


void PrintStream::println(Util::Time::Date date) {
    print(date);
    println();
}

PrintStream& PrintStream::operator<<(Util::Time::Date date) {
    print(date);
    return *this;
}


void PrintStream::printFormatted(const char* format, Util::Time::Date date) {
    for (; *format; format++) {
        if (*format != '%') {
            (*this) << *format;
        } else {
            format++;
            switch (*format) {
                case '%':
                    (*this) << '%';
                    break;

                case 'Y':
                    (*this) << date.getYear();
                    break;

                case 'y':
                    (*this) << (date.getYear())%100;
                    break;

                case 'b':
                    (*this) << monthAbbreviations[date.getMonth()];
                    break;

                case 'B':
                    (*this) << monthNames[date.getMonth()];
                    break;

                case 'm':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getMonth();
                    (*this).setIntegerPrecision(-1);
                    break;

                case 'U':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getWeekOfYearSunday();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'W':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getWeekOfYear();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'j':
                    (*this).setIntegerPrecision(3);
                    (*this) << date.getDayOfYear();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'd':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getDayOfMonth();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'a':
                    (*this) << weekdayAbbreviations[date.getWeekday()];
                    break;

                case 'A':
                    (*this) << weekdayNames[date.getWeekday()];
                    break;

                case 'w':
                    (*this) << (date.getWeekday()+1)%7;
                    break;

                case 'H':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getHours();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'I':
                    (*this).setIntegerPrecision(2);
                    (*this) << ((date.getHours() - 1) % 12) + 1;
                    (*this).setIntegerPrecision(0);
                    break;

                case 'M':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getMinutes();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'S':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getSeconds();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'c':
                    (*this).print(date);
                    break;

                case 'x':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getDayOfMonth() << "." << date.getMonth() << "." << date.getYear();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'X':
                    (*this).setIntegerPrecision(2);
                    (*this) << date.getHours() << ":" << date.getMinutes() << ":" << date.getSeconds();
                    (*this).setIntegerPrecision(0);
                    break;

                case 'p':
                    if (date.getHours() >= 13) (*this) << "p.m.";
                    else (*this) << "a.m.";
                    break;

                case 'Z':
                    (*this) << "UTC+0";
                    break;

                default:
                    break;
            }
        }
    }
}

}