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

#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
    #include "pico/util/datetime.h"
#endif

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

struct sDay
{
    int8_t day;
    int8_t month;
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
    void busTime_loop();
    void busTime_processReadRequests();
#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
    // Experimental Inclusion of UTC-Timer in RP2040
    void setHardwareDateTime(tm *iDateTime);
    void setHardwareDateTime(datetime_t t);
    void setHardwareValidDateTime();
#endif


  protected:
    static const uint8_t cHolidaysCount = 34;
    static sDay cHolidays[cHolidaysCount];
    struct tm mTimeHelper;
    // double mLongitude;
    // double mLatitude;
    // int8_t mTimezone;
    bool mUseSummertime;
    bool mIsSummertime;
    eTimeValid mTimeValid = tmInvalid;
    uint32_t mTimeDelay = 0;
    bool mMinuteChanged = false;
    uint8_t mHolidayToday = 0;
    uint8_t mHolidayTomorrow = 0;
    bool mHolidayChanged = false;
    sTime mSunrise;
    sTime mSunset;
    sDay mEaster = {0, 0};   // easter sunday
    sDay mAdvent = {0, 0};   // fourth advent
    int8_t mMinuteTick = -1; // timer evaluation is called each time the minute changes
    int8_t mHourTick = -1;   // timer evaluation is called each time the hour changes
    int8_t mDayTick = -1;    // sunrise/sunset calculation happens each time the day changes
    int8_t mMonthTick = -1;  // sunrise/sunset calculation happens each time the month changes
    int16_t mYearTick = -1;  // easter calculation happens each time year changes

    void calculateEaster();
    void calculateAdvent();
    bool calculateSummertime();
    uint8_t calculateLastSundayInMonth(uint8_t iMonth);
    void calculateHolidays(bool iDebugOutput = false);
    void calculateSunriseSunset();
    void convertToLocalTime(double iTime, sTime *eTime);
    bool isEqualDate(sDay &iDate1, sDay &iDate2);
    sDay getDayByOffset(int8_t iOffset, sDay &iDate);

    Timer();
    ~Timer();
    Timer(const Timer &);            // make copy constructor private
    Timer &operator=(const Timer &); // prevent copy

    int sunRiseSet(int year, int month, int day, double lon, double lat,
                   double altit, int upper_limb, double *rise, double *set);
    void sunPos(double d, double *lon, double *r);
    void sunRadDec(double d, double *RA, double *dec, double *r);
    double revolution(double x);
    double rev180(double x);
    double GMST0(double d);

  public:
    // singleton!
    static Timer &instance();
    struct tm mNow;
    float mLongitude;
    float mLatitude;
    int8_t mTimezone;

    // use double iLongitude, double iLatitude, int8_t iTimezone, bool iUseSummertime from Parameters
    void setup(uint64_t iHolidayBitmask);
    void loop();
    void busTime_processInputKo(GroupObject &iKo);
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
    bool minuteChanged();      // true every minute
    void clearMinuteChanged(); // has to be cleared externally
    void setTimeFromBus(tm *iTime);
    void setDateFromBus(tm *iDate);
    void setDateTimeFromBus(tm *iDateTime);
    uint8_t holidayToday();
    uint8_t holidayTomorrow();
    bool holidayChanged();
    void clearHolidayChanged();
    eTimeValid isTimerValid();
    bool IsSummertime();
    void IsSummertime(bool iValue);
    bool UseSummertime();
};

/* A macro to compute the number of days elapsed since 2000 Jan 0.0 */
/* (which is equal to 1999 Dec 31, 0h UT)                           */

#define days_since_2000_Jan_0(y, m, d) \
    (367L * (y) - ((7 * ((y) + (((m) + 9) / 12))) / 4) + ((275 * (m)) / 9) + (d) - 730530L)

/* Some conversion factors between radians and degrees */

// #ifndef PI
// #define PI 3.1415926535897932384
// #endif

#define RADEG (180.0 / PI)
#define DEGRAD (PI / 180.0)

/* The trigonometric functions in degrees */

#define sind(x) sin((x) * DEGRAD)
#define cosd(x) cos((x) * DEGRAD)
#define tand(x) tan((x) * DEGRAD)

#define atand(x) (RADEG * atan(x))
#define asind(x) (RADEG * asin(x))
#define acosd(x) (RADEG * acos(x))
#define atan2d(y, x) (RADEG * atan2(y, x))
