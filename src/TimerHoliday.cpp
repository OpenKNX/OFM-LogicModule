#include "KnxHelper.h"
#include "TimerHoliday.h"
#include "Arduino.h"
#include <ctime>

sDay TimerHoliday::cHolidays[cHolidaysCount] = {
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
    {-28, ADVENT},
    {20, 9}};

const std::string TimerHoliday::logPrefix()
{
    return "LOG-Holiday";
}

void TimerHoliday::setup()
{
    // do not fetch just ParamLOG_Neujahr here, we need the whole bitfield (first is MSB)
    uint8_t *iData = knx.paramData(LOG_Neujahr);
    // "remove" all holidays not enabled in configuration
    for (uint8_t i = 0; i < cHolidaysCount; i++)
    {
        const uint8_t dayBitMask = 0x80 >> (i % 8);
        if ((iData[i/8] & dayBitMask) == 0)
        {
            cHolidays[i].month = REMOVED;
        }
    }
}

void TimerHoliday::updateDate(tm mNow, bool sendHolidays /* = true */)
{
    // year changed => month changed => day change
    if (mYearTick != mNow.tm_year)
    {
        // logDebugP("tick: year %d -> %d", mYearTick, mNow.tm_year);

        calculateEaster(mNow.tm_year + 1900);
        calculateAdvent(mNow.tm_year);

        mYearTick = mNow.tm_year;
        mMonthTick = -1;
    }
    if (mMonthTick != mNow.tm_mon)
    {
        // logDebugP("tick: month %d -> %d", mMonthTick, mNow.tm_mon);

        mMonthTick = mNow.tm_mon;
        mDayTick = -1;
    }
    if (mDayTick != mNow.tm_mday)
    {
        // logDebugP("tick: day %d -> %d", mDayTick, mNow.tm_mday);

        const bool holidaysChanged = calculateHolidays(mNow.tm_year, mNow.tm_mon + 1, mNow.tm_mday);
        if (holidaysChanged && sendHolidays)
        {
            // logDebugP("send holidays: %d, %d", holidayToday(), holidayTomorrow());
            sendHoliday();
        }

        mDayTick = mNow.tm_mday;
    }
}

sDay *TimerHoliday::getEaster()
{
    return &mEaster;
}

uint8_t TimerHoliday::holidayToday()
{
    return mHolidayToday;
}

uint8_t TimerHoliday::holidayTomorrow()
{
    return mHolidayTomorrow;
}

#pragma region LOG_TIME_CALC_SPECIAL_DAYS

void TimerHoliday::calculateAdvent(int tm_year)
{
    // calculates the 4th advent
    mTimeHelper.tm_year = tm_year;
    mTimeHelper.tm_mon = 11;
    mTimeHelper.tm_mday = 24;
    mTimeHelper.tm_hour = 12;
    mTimeHelper.tm_min = 0;
    mTimeHelper.tm_sec = 0;
    mktime(&mTimeHelper); //   -timezone;
    mAdvent.day = 24 - mTimeHelper.tm_wday;
    mAdvent.month = 12;
}

void TimerHoliday::calculateEaster(uint16_t lYear)
{
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

/*
void TimerHoliday::debug()
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
*/

bool TimerHoliday::calculateHolidays(uint16_t tm_year, int8_t month, int8_t day, bool iDebugOutput)
{
    // check if today or tomorrow is a holiday
    sDay lToday = {day, month};
    sDay lTomorrow = getDayByOffset(1, lToday, tm_year);
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
                lHoliday = getDayByOffset(cHolidays[i].day, mEaster, tm_year);
                break;
            case ADVENT:
                lHoliday = getDayByOffset(cHolidays[i].day, mAdvent, tm_year);
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
            if (lHoliday == lToday)
                lHolidayToday = i + 1;
            if (lHoliday == lTomorrow)
                lHolidayTomorrow = i + 1;
            if (lHolidayToday > 0 && lHolidayTomorrow > 0 && !iDebugOutput)
                break;
        }
    }
    bool changed = false;
    if (lHolidayToday != mHolidayToday)
    {
        mHolidayToday = lHolidayToday;
        changed = true;
    }
    if (lHolidayTomorrow != mHolidayTomorrow)
    {
        mHolidayTomorrow = lHolidayTomorrow;
        changed = true;
    }
    return changed;
}

sDay TimerHoliday::getDayByOffset(int8_t iOffset, sDay &iDate, uint16_t tm_year)
{
    mTimeHelper.tm_year = tm_year;
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

// send holiday information on bus
void TimerHoliday::sendHoliday()
{
    // write the newly calculated holiday information into KO (can be read externally)

    KoLOG_Holiday1.valueNoSend(holidayToday(), getDPT(VAL_DPT_5));
    KoLOG_Holiday2.valueNoSend(holidayTomorrow(), getDPT(VAL_DPT_5));
    if (ParamLOG_HolidaySend)
    {
        // and send it, if requested by application setting
        KoLOG_Holiday1.objectWritten();
        KoLOG_Holiday2.objectWritten();
    }
}