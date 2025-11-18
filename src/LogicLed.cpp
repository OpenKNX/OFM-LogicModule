#include "LogicLed.h"

LogicLed::LogicLed(LogicChannel *iChannel, uint8_t iIOIndex)
{
    mLogicChannel = iChannel;
    mIOIndex = iIOIndex;
}

void LogicLed::on(bool active) {
    mLogicChannel->processInternalInput(mIOIndex, active);
}