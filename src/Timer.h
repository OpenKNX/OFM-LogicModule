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

#include "TimerHoliday.h"


#define MINYEAR 2022

#define SUN_SUNRISE 0x00
#define SUN_SUNSET 0x01

#define REMOVED 0
#define EASTER -1
#define ADVENT -2

// Values for Summertime
#define VAL_STIM_FROM_KO 0
#define VAL_STIM_FROM_DPT19 1
#define VAL_STIM_FROM_INTERN 2

// DPT19 special flags
#define DPT19_FAULT 0x80
#define DPT19_WORKING_DAY 0x40
#define DPT19_NO_WORKING_DAY 0x20
#define DPT19_NO_YEAR 0x10
#define DPT19_NO_DATE 0x08
#define DPT19_NO_DAY_OF_WEEK 0x04
#define DPT19_NO_TIME 0x02
#define DPT19_SUMMERTIME 0x01

struct sTime
{
    int8_t minute;
    int8_t hour;
};

enum eTimeValid
{
    tmInvalid,
    tmMinutesValid,
    tmDateValid,
    tmValid
};

class Timer
{
  private:
    const std::string logPrefix();

  protected:
    TimerHoliday holiday;

    struct tm mTimeHelper;
    bool mUseSummertime;
    bool mIsSummertime;
    eTimeValid mTimeValid = tmInvalid;
    uint32_t mTimeDelay = 0;
    bool mMinuteChanged = false;
    sTime mSunrise;
    sTime mSunset;
    int8_t mMinuteTick = -1; // timer evaluation is called each time the minute changes
    int8_t mHourTick = -1;   // timer evaluation is called each time the hour changes
    int8_t mDayTick = -1;    // sunrise/sunset calculation happens each time the day changes
    int8_t mMonthTick = -1;  // sunrise/sunset calculation happens each time the month changes
    int16_t mYearTick = -1;  // easter calculation happens each time year changes

    void calculateSunriseSunset();
    void convertToLocalTime(double iTime, sTime *eTime);

    Timer();
    ~Timer();
    Timer(const Timer &);            // make copy constructor private
    Timer &operator=(const Timer &); // prevent copy

  public:
    // singleton!
    static Timer &instance();
    struct tm mNow;
    float mLongitude;
    float mLatitude;

    void setup();
    bool loop();
    void debug();

    uint8_t getDay();
    uint8_t getMonth();
    uint16_t getYear();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();
    uint8_t getWeekday();
    sTime *getSunInfo(uint8_t iSunInfo);
    void getSunDegree(uint8_t iSunInfo, double iDegree, sTime *eSun);
    sDay *getEaster();
    char *getTimeAsc();
    uint8_t holidayToday();
    uint8_t holidayTomorrow();
    eTimeValid isTimerValid();
};

