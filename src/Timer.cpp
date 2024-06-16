#include "Timer.h"
#include "SunRiSet.h"
#include "Arduino.h"
#include <ctime>

#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
    #ifndef ARDUINO_ARCH_RP2040
        #error Experimental Feature OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME is supported on RP2040 only!
    #endif
    // Experimental Inclusion of RTC-Timer in RP2040
    // @see https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rtc_example
    #include "hardware/rtc.h"
    #include "pico/util/datetime.h"
#endif

sDay Timer::cHolidays[cHolidaysCount] = {
    {1, 1},
    {6, 1},
    {-52, EASTER},
    {-48, EASTER},
    {-47, EASTER},
    {-46, EASTER},
    {8, 3},
    {-3, EASTER},
    {-2, EASTER},
    {0, EASTER},
    {1, EASTER},
    {1, 5},
    {39, EASTER},
    {49, EASTER},
    {50, EASTER},
    {60, EASTER},
    {8, 8},
    {15, 8},
    {3, 10},
    {31, 10},
    {1, 11},
    {-32, ADVENT},
    {-21, ADVENT},
    {-14, ADVENT},
    {-7, ADVENT},
    {0, ADVENT},
    {24, 12},
    {25, 12},
    {26, 12},
    {31, 12},
    {26, 10},
    {8, 12},
    {1, 8},
    {-28, ADVENT}};

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

void Timer::setup(uint64_t iHolidayBitmask)
{
    bool lTimezoneSign = ParamBASE_TimezoneSign;
    int8_t lTimezone = ParamBASE_TimezoneValue;
    lTimezone = lTimezone * (lTimezoneSign ? -1 : 1);
    bool iUseSummertime = (ParamBASE_SummertimeAll == VAL_STIM_FROM_INTERN);

    mLongitude = ParamBASE_Longitude;
    mLatitude = ParamBASE_Latitude;
    mTimezone = ParamBASE_Timezone;
    mUseSummertime = iUseSummertime;
    // we delete all unnecessary holidays from holiday data
    for (uint8_t i = 0; i < cHolidaysCount; i++)
    {
        if ((iHolidayBitmask & 0x8000000000000000) == 0)
            cHolidays[i].month = REMOVED;
        iHolidayBitmask <<= 1;
    }
}

bool Timer::UseSummertime()
{
    return mUseSummertime;
}

// TODO Move to BusTime
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

void Timer::busTime_loop()
{
    // TODO needed after Separation only
    if (!openknx.afterStartupDelay())
        return;

    busTime_processReadRequests();
}

void Timer::loop()
{
    busTime_loop();

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
            // prevent that a minute is missed, if an other hour is set with the same minute
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
            // Ensure that a changed month causes a recalculation of all static dates/times
            if (mMonthTick != mNow.tm_mon)
            {
                mMonthTick = mNow.tm_mon;
                mYearTick = -1;
                mDayTick = -1;
            }
            if (mYearTick != mNow.tm_year)
            {
                calculateEaster();
                calculateAdvent();
                calculateSummertime(); // initial summertime calculation if year changes
                calculateHolidays();
                mYearTick = mNow.tm_year;
                // TODO check setting `mDayTick = -1;` for cases when year changes only. E.g. initial
            }
            // important: Day calculations AFTER year calculations
            if (mDayTick != mNow.tm_mday)
            {
                calculateSunriseSunset();
                if (!mHolidayChanged)
                    calculateHolidays();
                mDayTick = mNow.tm_mday;
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

#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
// Experimental Inclusion of RTC-Timer in RP2040
void Timer::setHardwareDateTime(tm *iDateTime)
{
    datetime_t t = {
        .year = (int16_t)iDateTime->tm_year,
        .month = (int8_t)iDateTime->tm_mon,
        .day = (int8_t)iDateTime->tm_mday,
        .dotw = (int8_t)iDateTime->tm_wday, // 0=Sunday
        .hour = (int8_t)iDateTime->tm_hour,
        .min = (int8_t)iDateTime->tm_min,
        .sec = (int8_t)iDateTime->tm_sec,
    };
    setHardwareDateTime(t);
}

void Timer::setHardwareDateTime(datetime_t t)
{
    // Start the RTC
    if (!rtc_running())
    {
        rtc_init();
    }
    rtc_set_datetime(&t);

    // "clk_sys is >2000x faster than clk_rtc,
    // so datetime is not updated immediately when rtc_get_datetime() is called.
    // tbe delay is up to 3 RTC clock cycles (which is 64us with the default clock settings)"
    // [https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rtc_example]
    sleep_us(64);
}

void Timer::setHardwareValidDateTime()
{
    // Experimental Inclusion of RTC-Timer in RP2040
    if (mTimeValid == tmValid)
    {
        setHardwareDateTime(&mNow);
    }
}
#endif

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

#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
    // Experimental Inclusion of RTC-Timer in RP2040
    setHardwareValidDateTime();
#endif
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

#ifdef OPENKNX_EXPERIMENTAL_RP2040RTC_LOCALTIME
    // Experimental Inclusion of RTC-Timer in RP2040
    setHardwareValidDateTime();
#endif
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
    return &mEaster;
}

char *Timer::getTimeAsc()
{
    return asctime(&mNow);
}

uint8_t Timer::holidayToday()
{
    return mHolidayToday;
}

uint8_t Timer::holidayTomorrow()
{
    return mHolidayTomorrow;
}

bool Timer::holidayChanged()
{
    return mHolidayChanged;
}

void Timer::clearHolidayChanged()
{
    mHolidayChanged = false;
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

#pragma region LOG_TIME_CALC_SPECIAL_DAYS

void Timer::calculateAdvent()
{
    // calculates the 4th advent
    mTimeHelper.tm_year = mNow.tm_year;
    mTimeHelper.tm_mon = 11;
    mTimeHelper.tm_mday = 24;
    mTimeHelper.tm_hour = 12;
    mTimeHelper.tm_min = 0;
    mTimeHelper.tm_sec = 0;
    mktime(&mTimeHelper); //   -timezone;
    mAdvent.day = 24 - mTimeHelper.tm_wday;
    mAdvent.month = 12;
}

void Timer::calculateEaster()
{
    uint16_t lYear = getYear();
    uint8_t a = lYear % 19;
    uint8_t b = lYear % 4;
    uint8_t c = lYear % 7;

    uint8_t k = lYear / 100;
    uint8_t q = k / 4;
    uint8_t p = ((8 * k) + 13) / 25;
    uint8_t Egz = (38 - (k - q) + p) % 30; // Die Jahrhundertepakte
    uint8_t M = (53 - Egz) % 30;
    uint8_t N = (4 + k - q) % 7;

    uint8_t d = ((19 * a) + M) % 30;
    uint8_t e = ((2 * b) + (4 * c) + (6 * d) + N) % 7;

    // Ausrechnen des Ostertermins:
    if ((22 + d + e) <= 31)
    {
        mEaster.day = 22 + d + e;
        mEaster.month = 3;
    }
    else
    {
        mEaster.day = d + e - 9;
        mEaster.month = 4;

        // Zwei Ausnahmen berücksichtigen:
        if (mEaster.day == 26)
            mEaster.day = 19;
        else if ((mEaster.day == 25) && (d == 28) && (a > 10))
            mEaster.day = 18;
    }
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
    sDay lToday = {(int8_t)getDay(), (int8_t)getMonth()};
    sDay lTomorrow = getDayByOffset(1, lToday);
    uint8_t lHolidayToday = 0;
    uint8_t lHolidayTomorrow = 0;
    for (uint8_t i = 0; i < cHolidaysCount; i++)
    {
        sDay lHoliday = {REMOVED, REMOVED};
        switch (cHolidays[i].month)
        {
            case REMOVED:
                // do nothing
                break;
            case EASTER:
                lHoliday = getDayByOffset(cHolidays[i].day, mEaster);
                break;
            case ADVENT:
                lHoliday = getDayByOffset(cHolidays[i].day, mAdvent);
                // do nothing
                break;
            default:
                // constant holiday
                lHoliday = cHolidays[i];
                break;
        }
        if (lHoliday.month > REMOVED)
        {
            if (iDebugOutput)
                logInfo("LogicTimer", "%02d.%02d., ", lHoliday.day, lHoliday.month);
            if (isEqualDate(lHoliday, lToday))
                lHolidayToday = i + 1;
            if (isEqualDate(lHoliday, lTomorrow))
                lHolidayTomorrow = i + 1;
            if (lHolidayToday > 0 && lHolidayTomorrow > 0 && !iDebugOutput)
                break;
        }
    }
    if (lHolidayToday != mHolidayToday)
    {
        mHolidayToday = lHolidayToday;
        mHolidayChanged = true;
    }
    if (lHolidayTomorrow != mHolidayTomorrow)
    {
        mHolidayTomorrow = lHolidayTomorrow;
        mHolidayChanged = true;
    }
}

bool Timer::isEqualDate(sDay &iDate1, sDay &iDate2)
{
    return (iDate1.day == iDate2.day && iDate1.month == iDate2.month);
}

sDay Timer::getDayByOffset(int8_t iOffset, sDay &iDate)
{
    mTimeHelper.tm_year = mNow.tm_year;
    mTimeHelper.tm_mon = iDate.month - 1;
    mTimeHelper.tm_mday = iDate.day + iOffset;
    mTimeHelper.tm_hour = 12;
    mTimeHelper.tm_min = 0;
    mTimeHelper.tm_sec = 0;

    // save a little time, if we are for sure within same month
    if (mTimeHelper.tm_mday < 1 || mTimeHelper.tm_mday > 28)
        mktime(&mTimeHelper); //   -timezone;

    // time_t nt_seconds = mktime(&mTimeHelper);     //   -timezone;
    //  return gmtime(&nt_seconds);

    sDay lResult = {(int8_t)mTimeHelper.tm_mday, (int8_t)(mTimeHelper.tm_mon + 1)};
    return lResult;
}
