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
};

class TimerHoliday
{
  private:
    struct tm mTimeHelper;

  protected:
    static const uint8_t cHolidaysCount = 34;
    static sDay cHolidays[cHolidaysCount];

    // force update on first holiday calculation
    uint8_t mHolidayToday = 255;
    uint8_t mHolidayTomorrow = 255;

    sDay mEaster = {0, 0};   // easter sunday
    sDay mAdvent = {0, 0};   // fourth advent

    bool isEqualDate(sDay &iDate1, sDay &iDate2);
    sDay getDayByOffset(int8_t iOffset, sDay &iDate, uint16_t year);

  public:
    void calculateEaster(uint16_t lYear);
    void calculateAdvent(int tm_year);
    bool calculateHolidays(uint16_t year, int8_t month, int8_t day, bool iDebugOutput = false);

    void setup();
    // void loop();

    sDay *getEaster();
    char *getTimeAsc();
    uint8_t holidayToday();
    uint8_t holidayTomorrow();
    void sendHoliday();
};

