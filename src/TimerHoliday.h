#pragma once

/***********************************
 *
 * Do all time relevant processing in logicmodule
 *
 * *********************************/

#include "OpenKNX.h"
#include <ctime>
#include <math.h>
#include <stdint.h>

#define REMOVED 0
#define EASTER -1
#define ADVENT -2


struct sDay
{
    int8_t day;
    int8_t month;

    bool operator==(const sDay &other) const {
        return (day == other.day && month == other.month);
    }
};

class TimerHoliday
{
  private:
    struct tm mTimeHelper;

    int16_t mYearTick = -1;  // easter calculation happens each time year changes
    int8_t mMonthTick = -1;  // sunrise/sunset calculation happens each time the month changes
    int8_t mDayTick = -1;    // sunrise/sunset calculation happens each time the day changes

    const std::string logPrefix();

    void calculateEaster(uint16_t lYear);
    void calculateAdvent(int tm_year);

  protected:
    static const uint8_t cHolidaysCount = 35;
    static sDay cHolidays[cHolidaysCount];

    // force update on first holiday calculation
    uint8_t mHolidayToday = 255;
    uint8_t mHolidayTomorrow = 255;

    sDay mEaster = {0, 0};   // easter sunday
    sDay mAdvent = {0, 0};   // fourth advent

    sDay getDayByOffset(int8_t iOffset, sDay &iDate, uint16_t year);

  public:
    bool calculateHolidays(uint16_t year, int8_t month, int8_t day, bool iDebugOutput = false);

    void setup();
    // void loop();
    void updateDate(tm mNow, bool sendHolidays = true);

    sDay *getEaster();
    char *getTimeAsc();
    uint8_t holidayToday();
    uint8_t holidayTomorrow();
    void sendHoliday();
};

