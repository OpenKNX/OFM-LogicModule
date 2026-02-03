#include <algorithm>
#include "Logic.h"
#include "LogicFunction.h"
#include "OpenKNX.h"
#include "Timer.h"
#include "TimerRestore.h"

#include "ModuleVersionCheck.h"

Logic openknxLogic;

uint8_t Logic::sMagicWord[] = {0xAE, 0x49, 0xD2, 0x9F};

// TODO Common Time
Timer &Logic::sTimer = Timer::instance();                      // singleton

TimerRestore &Logic::sTimerRestore = TimerRestore::instance(); // singleton

uint16_t Logic::flashSize()
{
    // Version + Data (Channel * Inputs * (Dpt + Value))
    return 1 + (LOG_ChannelCount * 2 * 5);
}

const std::string Logic::name()
{
    return "Logic";
}

const std::string Logic::version()
{
    return MODULE_LogicModule_Version;
}

Logic::Logic()
{
    // init KoLookup (robustness)
    addKoLookup(0, 0, 0);
    mNumKoLookups = 0;
}

Logic::~Logic()
{
}

LogicChannel *Logic::getChannel(uint8_t iChannelIndex)
{
    return mChannel[iChannelIndex];
}

void Logic::addKoLookup(uint16_t iKoNumber, uint8_t iChannelId, uint8_t iIOIndex)
{
    // first implementation, in future we use sorted insert
    mKoLookup[mNumKoLookups].koNumber = iKoNumber;
    mKoLookup[mNumKoLookups].channelIndex = iChannelId;
    mKoLookup[mNumKoLookups].ioIndex = iIOIndex;
    if (mNumKoLookups < cCountKoLookups)
        mNumKoLookups++;
}

bool Logic::getKoLookup(uint16_t iKoNumber, sKoLookup **iKoLookup)
{
    sKoLookup *lIterator = *iKoLookup;
    uint16_t lIterationCount = 0;
    if (*iKoLookup == 0)
        lIterator = &mKoLookup[0];
    else
        lIterator++;
    while (lIterator->koNumber > 0 && lIterationCount < mNumKoLookups)
    {
        lIterationCount++;
        if (lIterator->koNumber == iKoNumber)
        {
            *iKoLookup = lIterator;
            return true;
        }
        else
        {
            lIterator++;
        }
    }
    return false;
}

void Logic::prepareChannels()
{
    logDebugP("prepareChannels");
    for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
        mChannel[lIndex]->prepareChannel();
}

// we trigger all associated internal inputs with the new value
void Logic::processAllInternalInputs(LogicChannel *iChannel, bool iValue)
{
    // search for any internal input associated to this channel
    for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
    {
        LogicChannel *lChannel = mChannel[lIndex];
        lChannel->processInternalInputs(iChannel->channelIndex(), iValue);
    }
}

void Logic::processAfterStartupDelay()
{
    logDebugP("afterStartupDelay");
    logIndentUp();

    if (ParamLOG_VacationRead)
        KoLOG_Vacation.requestObjectRead();

    prepareChannels();

    // TODO FIXME: Repeated Reading
    // TODO FIXME: Implementation for DPT19
    logIndentDown();
}

bool Logic::sendReadRequest(GroupObject &ko)
{
    // ensure, that we do not send too many read requests at the same time
    if (delayCheck(readRequestDelay, 300)) // 3 per second
    {
        // we handle input KO and we send only read requests, if KO is uninitialized
        if (!ko.initialized())
            ko.requestObjectRead();
        readRequestDelay = delayTimerInit();
        return true;
    }
    return false;
}

void Logic::readFlash(const uint8_t *iBuffer, const uint16_t iSize)
{
    if (iSize == 0) // first call - without data
        return;

    uint8_t lVersion = openknx.flash.readByte();
    if (lVersion != 1) // version unknown
    {
        logDebugP("Wrong version of flash data (%i)", lVersion);
        return;
    }

    uint8_t lMaxChannels = (iSize - 1) / (2 * 5);
    logDebugP("Reading channel data from flash (%i/%i)", lMaxChannels, mNumChannels);
    for (uint8_t lIndex = 0; lIndex < MIN(mNumChannels, lMaxChannels); lIndex++)
    {
        mChannel[lIndex]->restore();
    }
}

void Logic::writeFlash()
{
    openknx.flash.writeByte(1); // Version
    for (uint8_t lIndex = 0; lIndex < MIN(mNumChannels, LOG_ChannelCount); lIndex++)
    {
        mChannel[lIndex]->save();
    }
}

// on input level, all dpt > 1 values are converted to bool by the according converter
void Logic::processInputKo(GroupObject &iKo)
{
    // we have to check first, if external KO are used
    sKoLookup *lKoLookup = nullptr;
    while (getKoLookup(iKo.asap(), &lKoLookup))
    {
        LogicChannel *lChannel = mChannel[lKoLookup->channelIndex];
        lChannel->processInput(lKoLookup->ioIndex);
    }
    // REVIEW: Wäre dieser Check nicht im LogicChannel besser aufgehoben?
    // Nein, denn dann müsste man alle channels durchgehen, um den richtigen zu finden
    // So wird nur der Kanal berechnet, der dann final prozessiert wird.
    if (iKo.asap() >= LOG_KoOffset + LOG_KoKOfE1 && iKo.asap() < LOG_KoOffset + LOG_KoKOfE1 + mNumChannels * LOG_KoBlockSize)
    {
        uint16_t lKoNumber = iKo.asap() - LOG_KoOffset - LOG_KoKOfE1;
        uint8_t lChannelId = lKoNumber / LOG_KoBlockSize;
        uint8_t lIOIndex = lKoNumber % LOG_KoBlockSize + 1;
        LogicChannel *lChannel = mChannel[lChannelId];
        lChannel->processInput(lIOIndex);
    }
}

const char* Logic::helpCommands[Logic::helpCommandCount] = {"logic help", "logic time", "logic easter", "logic sun", "logic sun+DDMM", "logic lim", "logic lim res", "logic chNN", "logic chNN lim", "logic chNN res"};

void Logic::showHelp()
{
    if (!knx.configured())
        return;

    // TODO Common Time {{{
    openknx.console.printHelpLine("logic time", "Print current time");
    openknx.console.printHelpLine("logic easter", "Print calculated easter Sunday date");
    openknx.console.printHelpLine("logic sun", "Print sunrise and sunset times");
    openknx.console.printHelpLine("logic sun+DDMM", "Print sunrise/sunset at elevation +/- degree/minute");
    // }}} TODO Common Time

    openknx.console.printHelpLine("logic lim", "Show call limit counter max value and channel");
    openknx.console.printHelpLine("logic lim res", "Reset all call limit counter");
    openknx.console.printHelpLine("logic chNN", "List logic channel NN, i.e. logic ch05");
    openknx.console.printHelpLine("logic chNN lim", "Show if this cannel reached call limit");
    openknx.console.printHelpLine("logic chNN res", "Resets call limit counter for this channel");
}


bool Logic::processCommand(const std::string iCmd, bool iDebugKo)
{
    bool lResult = false;

    if (!knx.configured())
        return lResult;

    if (iCmd.substr(0, 6) != "logic " || iCmd.length() < 7)
        return lResult;

    if (iCmd.length() >= 10 && iCmd.substr(6, 2) == "ch")
    {
        // Command ch<nn>: Logic inputs and output of last execution
        // find channel and dispatch
        uint16_t lIndex = std::stoi(iCmd.substr(8, 2)) - 1;
        if (lIndex < ParamLOG_VisibleChannels)
        {
            lResult = mChannel[lIndex]->processCommand(iCmd, iDebugKo);
        }
    }
    // TODO Common Time {{{
    else if (iCmd.length() >= 7 && iCmd.substr(6, 1) == "t") // time
    {
        // return internal time (might differ from external)
        uint8_t lHour = sTimer.getHour();
        uint8_t lMinute = sTimer.getMinute();
        uint8_t lSecond = sTimer.getSecond();
        uint8_t lDay = sTimer.getDay();
        uint8_t lMonth = sTimer.getMonth();
        uint16_t lYear = sTimer.getYear();
        // this if prevents stupid warnings
        if (lHour < 24 && lMinute < 60 && lSecond < 60 && lDay < 32 && lMonth < 13)
        {
            logInfoP("Module ISO-Time: %04d-%02d-%02d %02d:%02d:%02d", lYear, lMonth, lDay, lHour, lMinute, lSecond);
            if (iDebugKo)
            {
                openknx.console.writeDiagnoseKo("%02d:%02d:%02d %02d.%02d", lHour, lMinute, lSecond, lDay, lMonth);
            }
        }
        lResult = true;
    }
    else if (iCmd.length() >= 9 && iCmd.substr(6, 3) == "sun")
    {
        if (iCmd.length() == 14 && (iCmd.substr(9, 1) == "-" || iCmd.substr(9, 1) == "+"))
        {
            // return sunrise and sunset for a specific elevation teSDD,
            // where S=Sign(+,-) and DD ist elevation in degree
            double lDegree = std::stod(iCmd.substr(9, 3));
            uint8_t lMinute = std::stoi(iCmd.substr(12, 2));
            lDegree = (lDegree + lMinute / 60.0);
            sTime lSunrise;
            sTime lSunset;
            sTimer.getSunDegree(SUN_SUNRISE, lDegree, &lSunrise);
            sTimer.getSunDegree(SUN_SUNSET, lDegree, &lSunset);
            // this if prevents stupid warnings
            if (lSunrise.hour >= 0 && lSunrise.hour < 24 && lSunrise.minute >= 0 && lSunrise.minute < 60 && lSunset.hour >= 0 && lSunset.hour < 24 && lSunset.minute >= 0 && lSunset.minute < 60)
            {
                logInfoP("Sunrise: %02d:%02d Sunset: %02d:%02d", lSunrise.hour, lSunrise.minute, lSunset.hour, lSunset.minute);
                if (iDebugKo)
                {
                    openknx.console.writeDiagnoseKo("R%02d:%02d S%02d:%02d", lSunrise.hour, lSunrise.minute, lSunset.hour, lSunset.minute);
                }
                lResult = true;
            }
        }
        else if (iCmd.length() == 9)
        {
            // return sunrise and sunset
            sTime *lSunrise = sTimer.getSunInfo(SUN_SUNRISE);
            sTime *lSunset = sTimer.getSunInfo(SUN_SUNSET);
            // this if prevents stupid warnings
            if (lSunrise->hour >= 0 && lSunrise->hour < 24 && lSunrise->minute >= 0 && lSunrise->minute < 60 && lSunset->hour >= 0 && lSunset->hour < 24 && lSunset->minute >= 0 && lSunset->minute < 60)
            {
                logInfoP("Sunrise: %02d:%02d Sunset: %02d:%02d", lSunrise->hour, lSunrise->minute, lSunset->hour, lSunset->minute);
                if (iDebugKo)
                    openknx.console.writeDiagnoseKo("R%02d:%02d S%02d:%02d", lSunrise->hour, lSunrise->minute, lSunset->hour, lSunset->minute);
                lResult = true;
            }
        }
        else
        {
            logInfoP("Command format is logic sun[+-]DDMM");
            if (iDebugKo)
                openknx.console.writeDiagnoseKo("-> sun+-DDMM");
        }
    }
    else if (iCmd.length() >= 7 && iCmd.substr(6, 1) == "e") // easter
    {
        // calculate easter date
        logInfoP("Easter date: %02d.%02d", sTimer.getEaster()->day, sTimer.getEaster()->month);
        if (iDebugKo)
            openknx.console.writeDiagnoseKo("O%02d.%02d", sTimer.getEaster()->day, sTimer.getEaster()->month);
        lResult = true;
    }
    // }}} TODO Common Time
    else if (iCmd.length() >= 7 && iCmd.length() <= 9 && iCmd.substr(6, 1) == "l") // limit
    {
        // display max call limit and according channel
        logInfoP("Call limit %02d on channel %02d", LogicChannel::pLoadCounterMax, LogicChannel::pLoadChannel + 1);
        if (iDebugKo)
            openknx.console.writeDiagnoseKo("LIM %02d, CH %02d", LogicChannel::pLoadCounterMax, LogicChannel::pLoadChannel + 1);
        lResult = true;
    }
    else if (iCmd.length() >= 7 && iCmd.substr(6, 5) == "lim r") // limit
    {
        initLoadCounter(true);
        logInfoP("All call counters were reset!");
        if (iDebugKo)
            openknx.console.writeDiagnoseKo("Reset all");
        lResult = true;
    }
    else if (iCmd.length() >= 7 && iCmd.substr(6, 1) == "h") // help
    {
        showHelp();
        if (iDebugKo)
        {
            openknx.console.writeDiagnoseKo("-> time");
            openknx.console.writeDiagnoseKo(""); // workaround, on mass output each 2nd line ist skipped
            openknx.console.writeDiagnoseKo("-> easter");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> sun");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> sun[+-]DDMM");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> lim");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> lim res");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> chNN");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> chNN lim");
            openknx.console.writeDiagnoseKo("");
            openknx.console.writeDiagnoseKo("-> chNN res");
        }
        lResult = true;
    }
    return lResult;
}

void Logic::initLoadCounter(bool iAll)
{
    LogicChannel::pLoadCounterMax = 0;
    LogicChannel::pLoadChannel = 0;
    uint8_t lChannel;
    for (lChannel = 0; lChannel < mNumChannels; lChannel++)
    {
        if (iAll)
        {
            mChannel[lChannel]->pLoadCounter = 0;
        }
        else if (mChannel[lChannel]->pLoadCounter >= LOAD_COUNTER_MAX)
        {
            LogicChannel::pLoadCounterMax = LOAD_COUNTER_MAX;
            LogicChannel::pLoadChannel = lChannel;
            break;
        }
    }
}

void Logic::debug()
{
    // TODO Common Time {{{
    // // logInfoP("Aktuelle Zeit: %s", sTimer.getTimeAsc());
    // sTimer.debug();
    // }}} TODO Common Time
#ifdef ARDUINO_ARCH_RP2040
    // logInfoP("Free Heap: %i", rp2040.getFreeHeap());
#endif
}

void Logic::setup()
{
    sTimer.setup();

    // Number of available channels is the minimum of configured and available channels
    mNumChannels = MIN(ParamLOG_VisibleChannels, LOG_ChannelCount);

    // setup channels, not possible in constructor, because knx is not configured there
    // for TimerRestore we prepare all Timer channels
    for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
    {
        LogicChannel *lChannel = new LogicChannel(lIndex);
        mChannel[lIndex] = lChannel;
        lChannel->startTimerRestoreState();
        lChannel->prepareInternalInput(BIT_INT_INPUT_1, LOG_fI1);
        lChannel->prepareInternalInput(BIT_INT_INPUT_2, LOG_fI2);
    }
}

void Logic::loop()
{
    // TODO Common-Time: Check Using flag
    if (!openknx.afterStartupDelay())
        return;
    uint32_t lLoopTime = millis();

    // TODO Common-Time: Check Starting loop before startup Delay...
    if (sTimer.loop())
    {
        // special case timer handling, we have to loop through all channels once
        for (uint8_t lChannelNr = 0; lChannelNr < mNumChannels; lChannelNr++)
        {
            LogicChannel *lChannel = mChannel[lChannelNr];
            lChannel->startTimerInput();
        }
    }


    // we loop on all channels and execute pipeline
    uint8_t lChannelsProcessed = 0;
    // for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
    while (lChannelsProcessed < mNumChannels && openknx.freeLoopTime())
    {
        LogicChannel *lChannel = mChannel[mChannelIterator++];
        lChannel->loop();
        lChannelsProcessed++;
        // the following operations are done only once after iteration of all channels
        if (mChannelIterator >= mNumChannels)
        {
            mChannelIterator = 0;
            // here we do actions which happen after all channels are iterated
            processTimerRestore();
        }
    }
    if (lChannelsProcessed < mNumChannels)
        logTraceP("loop time reduced after %i channels", lChannelsProcessed);
    if (millis() - lLoopTime > 4)
        logTraceP("LoopTime: %i", millis() - lLoopTime);
}

// start timer implementation
void Logic::processTimerRestore()
{
    static uint32_t sTimerRestoreDelay = 1;
    // if (!knx.configured())
    //     return;

    if (sTimerRestoreDelay == 0)
        return;
    if (sTimer.isTimerValid() == tmValid && delayCheck(sTimerRestoreDelay, 500))
    {
        sTimerRestoreDelay = millis();
        if (sTimerRestoreDelay == 0)
            sTimerRestoreDelay = 1; // prevent set to 0 in case of timer overflow
        if (sTimerRestore.getDayIteration() < 365)
        {
            if (sTimerRestore.getDayIteration() == 0)
            {
                // initialize RestoreTimer
                sTimerRestore.setup(sTimer);
            }
            else
            {
                sTimerRestore.decreaseDay();
            }
            // loopSubmodules();
        }
        else
        {
            // stop timer restore processing in logic...
            sTimerRestoreDelay = 0;
            // ... and in each channel
            for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
            {
                LogicChannel *lChannel = mChannel[lIndex];
                lChannel->stopTimerRestoreState();
            }
        }
    }
}

tm &Logic::Now()
{
    return sTimer.mNow;
}

bool Logic::timeValid()
{
    return sTimer.isTimerValid();
}

bool Logic::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if (!knx.configured() || objectIndex != 160 || propertyId != 4)
        return false;

    switch (data[0])
    {
        case 1:
            LogicFunction::handleFunctionPropertyCheckFormula(data, resultData, resultLength);
            return true;
        case 2:
            LogicFunction::handleFunctionPropertyTestFormula(data, resultData, resultLength);
            return true;
        default:
            return false;
    }
}

