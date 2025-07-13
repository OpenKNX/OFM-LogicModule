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

/**
 * Update the internal timer.
 * @return if time was changed and depending updates should be triggered
 */
bool Timer::loop()
{
    bool lMinuteChanged = false;

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
        }
        if (mTimeValid == tmValid)
        {
            
            // year changed => month changed => day change => hour changed => minute changed (=> second changed)
            if (mYearTick != mNow.tm_year)
            {
                logDebugP("tick: year %d -> %d", mYearTick, mNow.tm_year);

                // update of easter and advend is triggered in holiday.updateDate(mNow)

                mYearTick = mNow.tm_year;
                mMonthTick = -1;
            }
            if (mMonthTick != mNow.tm_mon)
            {
                logDebugP("tick: month %d -> %d", mMonthTick, mNow.tm_mon);

                // TZ and DST handled in Common Time only

                mMonthTick = mNow.tm_mon;
                mDayTick = -1;
            }
            if (mDayTick != mNow.tm_mday)
            {
                logDebugP("tick: day %d -> %d", mDayTick, mNow.tm_mday);

                calculateSunriseSunset();

                // set current date to trigger recalc of holiday related days and update/send GOs for today/tomorrow
                holiday.updateDate(mNow);

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
            }
        }
    }
    return lMinuteChanged;
}

// TODO: Check Migration to Common Time / SunCalculation
void Timer::convertToLocalTime(double iTime, sTime *eTime)
{
    OpenKNX::TimeOnly localTime = OpenKNX::DateTime(
        getYear(),
        getMonth(),
        getDay(), 
        (int)floor(iTime), 
        (int)(60 * (iTime - floor(iTime))), 
        0, 
        OpenKNX::DateTimeTypeUTC
    ).toLocalTime();

    eTime->hour = localTime.hour;
    eTime->minute = localTime.minute;
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
        holiday.calculateHolidays(mNow.tm_year, getMonth(), getDay(), true);
        logInfo("LogicTimer", "\nEnd of holiday debug\n");
        logInfo("LogicTimer", "Sonnenaufgang: %02d:%02d, Sonnenuntergang: %02d:%02d\n\n", mSunrise.hour, mSunrise.minute, mSunset.hour, mSunset.minute);
    }
#endif
}
