/**
 * Ein Treiber für die Realtime Clock (Echtzeituhr).
 */
#ifndef __RTC__include__
#define __RTC__include__

#define RTC_DEBUG 0

#include "kernel/IOport.h"
#include "IODevice.h"

/**
 * Driver for the CMOS Realtime clock.
 */
class Rtc : public IODevice {

public:
    /**
     * Contains Date- and Time-information.
     */
    struct Date {
        uint8_t seconds = 0;
        uint8_t minutes = 0;
        uint8_t hours = 0;
        uint8_t dayOfMonth = 0;
        uint8_t month = 0;
        uint16_t year = 0;

        bool operator == (Date &date) {
            return seconds == date.seconds && minutes == date.minutes && hours == date.hours &&
                   dayOfMonth == date.dayOfMonth && month == date.month && year == date.year;
        }

        bool operator != (Date &date) {
            return seconds != date.seconds || minutes != date.minutes || hours != date.hours ||
                   dayOfMonth != date.dayOfMonth || month != date.month || year != date.year;
        }
    };

private:
    /**
     * RTC-Registers.
     */
    enum REGISTERS {
        SECONDS_REGISTER = 0x00,
        MINUTES_REGISTER = 0x02,
        HOURS_REGISTER = 0x04,
        WEEKDAY_REGISTER = 0x06,
        DAY_OF_MONTH_REGISTER = 0x07,
        MONTH_REGISTER = 0x08,
        YEAR_REGISTER = 0x09,
        CENTURY_REGISTER = 0x32,
        STATUS_REGISTER_A = 0x8A,
        STATUS_REGISTER_B = 0x8B,
        STATUS_REGISTER_C = 0x0C
    };

    IOport registerPort, dataPort;

    Rtc::Date currentDate {};

    static const char RTC_RATE = 0x06;

public:
    /**
     * Constructor.
     */
    Rtc();

    /**
     * Copy-constructor.
     */
    Rtc(const Rtc &copy) = delete;

    /**
     * Destructor.
     */
    ~Rtc() override = default;

    /**
     * Enable periodic interrupts for the RTC.
     */
    void plugin();

    /**
     * Overriding function from IODevice.
     */
    void trigger() override;

    /**
     * Overriding function from IODevice.
     */
    bool checkForData() override;

    /**
     * Get the current date.
     */
    Date getCurrentDate();
};

#endif
