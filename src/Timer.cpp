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
    mNow.tm_year = 120;
    mNow.tm_mon = 0;
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

void Timer::setup()
{
    mLongitude = ParamBASE_Longitude;
    mLatitude = ParamBASE_Latitude;

    // TODO Common Time {{{
    mTimezone = ParamBASE_TimezoneValue * (ParamBASE_TimezoneSign ? -1 : 1);
    mUseSummertime = (ParamBASE_SummertimeAll == VAL_STIM_FROM_INTERN);
    // TODO Common Time }}}

    holiday.setup();
}

bool Timer::UseSummertime()
{
    return mUseSummertime;
}

/*
void Timer::busTime_processReadRequests()
{
    static uint32_t sDelay = 19000;

    // date and time are red from bus every 30 seconds until a response is received
    if (ParamBASE_ReadTimeDate)
    {
        const eTimeValid lValid = isTimerValid();
        if (delayCheck(sDelay, 30000) && lValid != tmValid)
        {
            // logDebugP("Time Valid? %i", lValid);
            sDelay = millis();
            if (ParamBASE_CombinedTimeDate)
            {
                // combined date and time
                KoBASE_Time.requestObjectRead();
            }
            else
            {
                // date and time from separate KOs
                if (lValid != tmMinutesValid)
                    KoBASE_Time.requestObjectRead();
                if (lValid != tmDateValid)
                    KoBASE_Date.requestObjectRead();
            }
        }
        // if date and/or time is known, we read also summertime information
        // TODO check dependency to configuration
        if (sDelay > 0 && lValid == tmValid)
        {
            sDelay = 0;
            KoBASE_IsSummertime.requestObjectRead();
        }
    }
}
*/

void Timer::loop()
{
    /* TODO check complete removal, as read requests are send by new common time implementation
    if (openknx.afterStartupDelay())
        busTime_processReadRequests();
    */

    if (mTimeDelay == 0 || delayCheck(mTimeDelay, 1000))
    {
        // if time is set from bus, we have immediately to recalculate everything which is necessary
        if (mTimeDelay == 0)
            mTimeDelay = millis();
        else
        {
            mTimeDelay += 1000;
            mNow.tm_sec += 1;
            mktime(&mNow);
        }
        if (mTimeValid == tmValid)
        {
            
            // year changed => month changed => day change => hour changed => minute changed (=> second changed)
            if (mYearTick != mNow.tm_year)
            {
                holiday.calculateEaster(getYear());
                holiday.calculateAdvent(mNow.tm_year);

                mYearTick = mNow.tm_year;
                mMonthTick = -1;
            }
            if (mMonthTick != mNow.tm_mon)
            {
                calculateSummertime(); // initial summertime calculation if year changes
                calculateHolidays();

                mMonthTick = mNow.tm_mon;
                mDayTick = -1;
            }
            // important: Day calculations AFTER year calculations
            if (mDayTick != mNow.tm_mday)
            {
                calculateSunriseSunset();
                if (!holiday.holidayChanged())
                    calculateHolidays();

                mDayTick = mNow.tm_mday;
                mHourTick = -1;
            }
            if (mHourTick != mNow.tm_hour)
            {
                mHourTick = mNow.tm_hour;
                mMinuteTick = -1;
            }
            if (mMinuteTick != mNow.tm_min)
            {
                mMinuteChanged = true;
                // just call once a minute
                mMinuteTick = mNow.tm_min;

                if (mUseSummertime && (getMonth() == 3 || getMonth() == 10) && getHour() == 3 && getMinute() == 1)
                    calculateSummertime();
            }
        }
    }
}

void Timer::busTime_processInputKo(GroupObject &iKo)
{
    if (iKo.asap() == BASE_KoTime)
    {
        if (ParamBASE_CombinedTimeDate)
        {
            KNXValue value = "";

            // first ensure we have a valid data-time content
            // (including the correct length)
            if (iKo.tryValue(value, DPT_DateTime))
            {

                // use raw value, as current version of knx do not provide access to all fields
                // TODO DPT19: check integration of extended DPT19 access into knx or OpenKNX-Commons
                // size is ensured to be 8 Byte
                uint8_t *raw = iKo.valueRef();

                /*
                const bool flagFault = raw[6] & 0x80;
                // ignore working day (WD, NWD): raw[6] & 0x40, raw[6] & 0x20
                const bool flagNoYear = raw[6] & 0x10;
                const bool flagNoDate = raw[6] & 0x08;
                // ignore NDOW: raw[6] & 0x04
                const bool flagNoTime = raw[6] & 0x02;
                const bool flagSuti = raw[6] & 0x01;
                // ignore quality of clock (CLQ): raw[7] & 0x80
                // ignore synchronisation source reliablity (SRC): raw[7] & 0x40
                */

                // ignore inputs with:
                // * F - fault
                // * NY - missing year
                // * ND - missing date
                // * NT - missing time
                if (!(raw[6] & (DPT19_FAULT | DPT19_NO_YEAR | DPT19_NO_DATE | DPT19_NO_TIME)))
                {
                    struct tm lTmp = value;
                    setDateTimeFromBus(&lTmp);
                    const bool lSummertime = raw[6] & DPT19_SUMMERTIME;
                    if (ParamBASE_SummertimeAll == VAL_STIM_FROM_DPT19)
                        IsSummertime(lSummertime);
                }
            }
        }
        else
        {
            KNXValue value = "";
            // ensure we have a valid time content
            if (iKo.tryValue(value, DPT_TimeOfDay))
            {
                struct tm lTmp = value;
                setTimeFromBus(&lTmp);
            }
        }
    }
    else if (iKo.asap() == BASE_KoDate)
    {
        KNXValue value = "";
        // ensure we have a valid date content
        if (iKo.tryValue(value, DPT_Date))
        {
            struct tm lTmp = value;
            setDateFromBus(&lTmp);
        }
    }
    else if (iKo.asap() == BASE_KoIsSummertime)
    {
        IsSummertime(iKo.value(DPT_Date));
    }
}

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

void Timer::setTimeFromBus(tm *iTime)
{
    if (mNow.tm_min != iTime->tm_min || mNow.tm_hour != iTime->tm_hour)
        mMinuteChanged = true;
    mNow.tm_sec = iTime->tm_sec;
    mNow.tm_min = iTime->tm_min;
    mNow.tm_hour = iTime->tm_hour;
    mktime(&mNow);
    mTimeDelay = 0; // force time/year calculations
    mTimeValid = static_cast<eTimeValid>(mTimeValid | tmMinutesValid);
}

void Timer::setDateFromBus(tm *iDate)
{
    // we have to check, if some date dependant calculations have to be done
    // in case of date changes
    if (iDate->tm_year != getYear())
    {
        mYearTick = -1; // triggers easter calculation
        mDayTick = -1;  // triggers sunrise/sunset calculation
        mMinuteChanged = true;
    }
    else if (iDate->tm_mon != getMonth() || iDate->tm_mday != getDay())
    {
        mDayTick = -1; // triggers sunrise/sunset calculation
        mMinuteChanged = true;
    }
    mNow.tm_mday = iDate->tm_mday;
    mNow.tm_mon = iDate->tm_mon - 1;
    mNow.tm_year = iDate->tm_year - 1900;
    mktime(&mNow);
    mTimeDelay = 0; // force time/year calculations
    if (mNow.tm_year >= MINYEAR - 1900)
        mTimeValid = static_cast<eTimeValid>(mTimeValid | tmDateValid);
}

void Timer::setDateTimeFromBus(tm *iDateTime)
{
    // TODO DPT19: check optimizations
    setTimeFromBus(iDateTime);
    setDateFromBus(iDateTime);

    // RTC is set from inside previous functions
}

bool Timer::minuteChanged()
{
    return mMinuteChanged && mTimeValid == tmValid;
}

void Timer::clearMinuteChanged()
{
    mMinuteChanged = false;
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

void Timer::calculateHolidays(bool iDebugOutput)
{
    // we check only if date is valid
    if (mTimeValid < tmDateValid)
        return;
    // check if today or tomorrow is a holiday
    // TODO use sDay lToday = {(int8_t)getDay(), (int8_t)getMonth()};
    holiday.calculateHolidays(mNow.tm_year, getMonth(), getDay(), iDebugOutput);
}

// send holiday information on bus
void Timer::sendHoliday()
{
    if (holiday.holidayChanged())
    {
        // write the newly calculated holiday information into KO (can be read externally)

        KoLOG_Holiday1.valueNoSend(holidayToday(), getDPT(VAL_DPT_5));
        KoLOG_Holiday2.valueNoSend(holidayTomorrow(), getDPT(VAL_DPT_5));
        holiday.clearHolidayChanged();
        if (ParamLOG_HolidaySend)
        {
            // and send it, if requested by application setting
            KoLOG_Holiday1.objectWritten();
            KoLOG_Holiday2.objectWritten();
        }
    }
}
