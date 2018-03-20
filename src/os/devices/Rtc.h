/**
 * Ein Treiber für die Realtime Clock (Echtzeituhr).
 */
#ifndef __RTC__include__
#define __RTC__include__

#define RTC_DEBUG 0

#define SECONDS_REGISTER        0x00
#define MINUTES_REGISTER        0x02
#define HOURS_REGISTER          0x04
#define WEEKDAY_REGISTER        0x06
#define DAY_OF_MONTH_REGISTER   0x07
#define MONTH_REGISTER          0x08
#define YEAR_REGISTER           0x09
#define CENTURY_REGISTER        0x32
#define STATUS_REGISTER_A       0x8A    // Disable NMI
#define STATUS_REGISTER_B       0x8B    // Disable NMI
#define STATUS_REGISTER_C       0x0C

#include "kernel/IOport.h"
#include "kernel/interrupts/InterruptHandler.h"

class Rtc : public InterruptHandler{

public:
    struct date {
        int seconds;
        int minutes;
        int hours;
        int dayOfMonth;
        int month;
        int year;
    };

private:
    IOport registerPort, dataPort;
    Rtc::date currentDate;
    bool isDateSet = false;

    static const char RTC_RATE = 0x06;

    // Copy Konstrutkor unterbinden
    Rtc(const Rtc &copy);

public:
    Rtc() : registerPort(0x70), dataPort(0x71) {}

    void plugin();

    void trigger();

    date getCurrentDate();
};

#endif
