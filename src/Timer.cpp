#include "KnxHelper.h"
#include "Timer.h"
#include "SunRiSet.h"
#include "Arduino.h"
#include <ctime>

#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
    #error OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME replaced by new OpenKNX Time Implementation!
#endif

Timer::Timer()
{
    mNow.tm_year = 2020 - 1900;
    mNow.tm_mon = 1 - 1;
    mNow.tm_mday = 1;
    mNow.tm_wday = 3;
    mktime(&mNow);
    mTimeDelay = millis();
}

Timer::~Timer()
{
}

Timer &Timer::instance()
{
    static Timer sInstance;
    return sInstance;
}

const std::string Timer::logPrefix()
{
    return "LOG-Time";
}

void Timer::setup()
{
    mLongitude = ParamBASE_Longitude;
    mLatitude = ParamBASE_Latitude;

    // TZ and DST handled in Common Time only

    holiday.setup();
}

// TODO Common Time: Remove
bool Timer::UseSummertime()
{
    return mUseSummertime;
}

/**
 * Update the internal timer.
 * @return if time was changed and depending updates should be triggered
 */
bool Timer::loop()
{
    bool lMinuteChanged = false;

    /* TODO check complete removal, as read requests are send by new common time implementation
    if (openknx.afterStartupDelay())
        busTime_processReadRequests();
    */

    // TODO Common Time: mTimeDelay=0 was set be Update from Bus
    // TODO mTimeDelay == 0 will be reached after 49 days...
    if (mTimeDelay == 0 || delayCheck(mTimeDelay, 1000))
    {
        // TODO Common Time: No special handling of set from bus, but try to detect missed second
        // if time is set from bus, we have immediately to recalculate everything which is necessary
        if (mTimeDelay == 0)
            // external update
            mTimeDelay = millis();
        else
        {
            // regular running of internal clock
            mTimeDelay += 1000;
            mNow.tm_sec += 1;
            mktime(&mNow);

            // TODO Common Time: check if time is expected
        }
        // always use current time from Common
        openknx.time.getLocalTime().toTm(mNow);
        if (openknx.time.isValid())
        {
            // TODO Common Time: Replace mTimeValid
            mTimeValid = tmValid;

            // TODO set IsSummertime(lSummertime);
        }
        if (mTimeValid == tmValid)
        {
            
            // year changed => month changed => day change => hour changed => minute changed (=> second changed)
            if (mYearTick != mNow.tm_year)
            {
                logDebugP("tick: year %d -> %d", mYearTick, mNow.tm_year);

                holiday.calculateEaster(getYear());
                holiday.calculateAdvent(mNow.tm_year);

                mYearTick = mNow.tm_year;
                mMonthTick = -1;
            }
            if (mMonthTick != mNow.tm_mon)
            {
                logDebugP("tick: month %d -> %d", mMonthTick, mNow.tm_mon);

                // TZ and DST handled in Common Time only
                /*
                calculateSummertime(); // initial summertime calculation if year changes
                */

                mMonthTick = mNow.tm_mon;
                mDayTick = -1;
            }
            if (mDayTick != mNow.tm_mday)
            {
                logDebugP("tick: day %d -> %d", mDayTick, mNow.tm_mday);

                calculateSunriseSunset();

                // TODO Common Time: Move into TimerHoliday
                if (calculateHolidays())
                {
                    logDebugP("send holidays: %d, %d", holiday.holidayToday(), holiday.holidayTomorrow());
                    holiday.sendHoliday();
                }

                mDayTick = mNow.tm_mday;
                mHourTick = -1;
            }
            if (mHourTick != mNow.tm_hour)
            {
                logDebugP("tick: hour %d -> %d", mHourTick, mNow.tm_hour);

                mHourTick = mNow.tm_hour;
                mMinuteTick = -1;
            }
            if (mMinuteTick != mNow.tm_min)
            {
                logDebugP("tick: minute %d -> %d", mMinuteTick, mNow.tm_min);

                lMinuteChanged = true;
                // just call once a minute
                mMinuteTick = mNow.tm_min;

                // TZ and DST handled in Common Time only
                /*
                if (mUseSummertime && (getMonth() == 3 || getMonth() == 10) && getHour() == 3 && getMinute() == 1)
                    calculateSummertime();
                */
            }
            // TODO Common Time: set mUseSummertime, mIsSummertime, mTimezone
        }
    }
    return lMinuteChanged;
}

// TODO Common Time: Remove
void Timer::convertToLocalTime(double iTime, sTime *eTime)
{
    eTime->hour = (int)floor(iTime);
    eTime->minute = (int)(60 * (iTime - floor(iTime)));
    eTime->hour += mTimezone + ((mIsSummertime) ? 1 : 0);
}

void Timer::calculateSunriseSunset()
{
    double rise, set;
    // sunrise/sunset calculation
    SunRiSet::sunRiseSet(getYear(), getMonth(), getDay(),
               mLongitude, mLatitude, -35.0 / 60.0, 1, &rise, &set);
    convertToLocalTime(rise, &mSunrise);
    convertToLocalTime(set, &mSunset);
}

#pragma region LOG_TIME_GETTER

uint16_t Timer::getYear()
{
    return mNow.tm_year + 1900;
}

uint8_t Timer::getMonth()
{
    return mNow.tm_mon + 1;
}

uint8_t Timer::getDay()
{
    return mNow.tm_mday;
}

uint8_t Timer::getHour()
{
    return mNow.tm_hour;
}

uint8_t Timer::getMinute()
{
    return mNow.tm_min;
}

uint8_t Timer::getSecond()
{
    return mNow.tm_sec;
}

uint8_t Timer::getWeekday()
{
    return mNow.tm_wday;
}

#pragma endregion

sTime *Timer::getSunInfo(uint8_t iSunInfo)
{
    if (iSunInfo == SUN_SUNRISE)
        return &mSunrise;
    else if (iSunInfo == SUN_SUNSET)
        return &mSunset;
    else
        return NULL;
}

void Timer::getSunDegree(uint8_t iSunInfo, double iDegree, sTime *eSun)
{
    double rise, set;
    // sunrise/sunset calculation
    SunRiSet::sunRiseSet(getYear(), getMonth(), getDay(),
               mLongitude, mLatitude, iDegree, 0, &rise, &set);
    if (iSunInfo == SUN_SUNRISE)
        convertToLocalTime(rise, eSun);
    else if (iSunInfo == SUN_SUNSET)
        convertToLocalTime(set, eSun);
}

sDay *Timer::getEaster()
{
    return holiday.getEaster();
}

char *Timer::getTimeAsc()
{
    return asctime(&mNow);
}

uint8_t Timer::holidayToday()
{
    return holiday.holidayToday();
}

uint8_t Timer::holidayTomorrow()
{
    return holiday.holidayTomorrow();
}

eTimeValid Timer::isTimerValid()
{
    return mTimeValid;
}

#pragma region LOG_TIME_DST

bool Timer::IsSummertime()
{
    return mIsSummertime;
}

void Timer::IsSummertime(bool iValue)
{
    if (iValue != mIsSummertime)
    {
        mIsSummertime = iValue;
        calculateSunriseSunset();
    }
}

uint8_t Timer::calculateLastSundayInMonth(uint8_t iMonth)
{
    mTimeHelper.tm_year = mNow.tm_year;
    mTimeHelper.tm_mon = iMonth - 1;
    mTimeHelper.tm_mday = 31;
    mktime(&mTimeHelper);
    return mTimeHelper.tm_mday - mTimeHelper.tm_wday;
}

// TODO Common Time: Remove
// should be called only at 03:01 o'clock
bool Timer::calculateSummertime()
{
    // first we do easy win
    bool lResult = false;
    if (mUseSummertime)
    {
        bool lIsSummertime = false;
        if (getMonth() == 3)
        {
            // find last Sunday in March
            uint8_t lLastSunday = calculateLastSundayInMonth(3);
            if (lLastSunday == mNow.tm_mday)
            {
                // we have to take time into account
                lIsSummertime = (mNow.tm_hour > 3);
            }
            else
            {
                lIsSummertime = (lLastSunday < mNow.tm_mday);
            }
        }
        else if (getMonth() == 10)
        {
            // find last Sunday in October
            uint8_t lLastSunday = calculateLastSundayInMonth(10);
            if (lLastSunday == mNow.tm_mday)
            {
                // we have to take time into account
                // here might be a problem if called between
                // 2 and 3, because these times exist in both
                // summer and wintertime. This is currently
                // mitigated with the fact, that this routine
                // is called only once at 03:01. Currently just used
                // for sunrise/sunset calculation, so calling
                // time is no problem.
                lIsSummertime = (mNow.tm_hour < 3);
            }
            else
            {
                lIsSummertime = (lLastSunday > mNow.tm_mday);
            }
        }
        else
        {
            lIsSummertime = (getMonth() > 3 && getMonth() < 10);
        }
        if (lIsSummertime != mIsSummertime)
        {
            IsSummertime(lIsSummertime);
            lResult = true;
        }
    }
    return lResult;
}

#pragma endregion

void Timer::debug()
{
    if (mTimeValid & tmMinutesValid)
    {
        logInfo("LogicTimer", "Aktuelle Zeit: %s", getTimeAsc());
    }
#if LOGIC_TRACE
    if (mTimeValid & tmDateValid)
    {
        logInfo("LogicTimer", "\nFeiertage %d: ", getYear());
        calculateHolidays(true);
        logInfo("LogicTimer", "\nEnd of holiday debug\n");
        logInfo("LogicTimer", "Sonnenaufgang: %02d:%02d, Sonnenuntergang: %02d:%02d\n\n", mSunrise.hour, mSunrise.minute, mSunset.hour, mSunset.minute);
    }
#endif
}

// TODO Common Time: Remove
// TODO Common Time: Recalc and send holidays based on date update
bool Timer::calculateHolidays(bool iDebugOutput)
{
    // we check only if date is valid // TODO check if needed in TimerRestore
    if (mTimeValid < tmDateValid)
        return false;

    // check if today or tomorrow is a holiday
    return holiday.calculateHolidays(mNow.tm_year, getMonth(), getDay(), iDebugOutput);
}

