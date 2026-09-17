#include "LogicFunction.h"
#include "OpenKNX.h"
#include "knx.h"

double LogicFunction::e1 = 0;
double LogicFunction::e2 = 0;
double LogicFunction::out = 0;
uint8_t LogicFunction::sRecursionCounter = 0;
char LogicFunction::sFormulaBuffer[100] = {0};

// native functions, implemented as a simple example how to use user functions
LogicValue LogicFunction::nativeAdd(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 + E2;
}

LogicValue LogicFunction::nativeSubtract(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 - E2;
}

LogicValue LogicFunction::nativeMultiply(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 * E2;
}

LogicValue LogicFunction::nativeDivide(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 / E2;
}

LogicValue LogicFunction::nativeAverage(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (float)(E1 + E2) / 2;
}

LogicValue LogicFunction::nativeMinimum(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (E1 < E2) ? E1 : E2;
}

LogicValue LogicFunction::nativeMaximum(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (E1 > E2) ? E1 : E2;
}

LogicValue LogicFunction::nativeModulo(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 % (int32_t)E2;
}

LogicValue LogicFunction::nativeAnd(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 & (int32_t)E2;
}

LogicValue LogicFunction::nativeOr(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 | (int32_t)E2;
}

LogicValue LogicFunction::nativeXor(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 ^ (int32_t)E2;
}

LogicValue LogicFunction::nativeLShift(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 << (int32_t)E2;
}

LogicValue LogicFunction::nativeRShift(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 >> (int32_t)E2;
}

LogicValue LogicFunction::nativeBool2Int(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    // we assume E1, E2 are boolean
    // if not, standard conversion applies
    return (uint8_t)((bool)E2 * 2 + (bool)E1);
}

LogicValue LogicFunction::nativeSmooth(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    if (E1.isInitial() || E2.isInitial())
        return 0.0f;
    if ((bool)E2 == 0)
        return 0.0f;
    if (iOld.isInitial())
        return E1;
    else
        return iOld + (E1 - iOld) / E2;
}

LogicValue LogicFunction::nativeIncrementE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 + LogicValue((uint8_t)1);
}

LogicValue LogicFunction::nativeIncrementE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E2 + LogicValue((uint8_t)1);
}

LogicValue LogicFunction::nativeIncrementA(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld + LogicValue((uint8_t)1);
}

LogicValue LogicFunction::nativeDecrementE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 - LogicValue((uint8_t)1);
}

LogicValue LogicFunction::nativeDecrementE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E2 - LogicValue((uint8_t)1);
}

LogicValue LogicFunction::nativeDecrementA(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld - LogicValue((uint8_t)1);
}

LogicValue LogicFunction::nativeAddE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld + E1;
}

LogicValue LogicFunction::nativeAddE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld + E2;
}

LogicValue LogicFunction::nativeAddE1PlusE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld + (E1 + E2);
}

LogicValue LogicFunction::nativeAddE1MinusE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld + (E1 - E2);
}

LogicValue LogicFunction::nativeSubtractE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld - E1;
}

LogicValue LogicFunction::nativeSubtractE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld - E2;
}

LogicValue LogicFunction::nativeSubtractE1PlusE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld - (E1 + E2);
}

LogicValue LogicFunction::nativeSubtractE1MinusE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return iOld - (E1 - E2);
}

LogicValue LogicFunction::nativeMinimumA(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    if (E1 < E2 && E1 < iOld)
        return E1;
    else if (E2 < E1 && E2 < iOld)
        return E2;
    else
        return iOld;
}

LogicValue LogicFunction::nativeMaximumA(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    if (E1 > E2 && E1 > iOld)
        return E1;
    else if (E2 > E1 && E2 > iOld)
        return E2;
    else
        return iOld;
}

LogicValue LogicFunction::nativeLShiftE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 << 1;
}

LogicValue LogicFunction::nativeLShiftE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E2 << 1;
}

LogicValue LogicFunction::nativeLShiftA(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)iOld << 1;
}

LogicValue LogicFunction::nativeRShiftE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E1 >> 1;
}

LogicValue LogicFunction::nativeRShiftE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)E2 >> 1;
}

LogicValue LogicFunction::nativeRShiftA(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return (int32_t)iOld >> 1;
}

LogicValue LogicFunction::nativePower(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    return E1 ^ E2;
}

LogicValue LogicFunction::nativeNegativeE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    LogicValue lMinusOne = (double)-1;
    return E1 * lMinusOne;
}

LogicValue LogicFunction::nativeNegativeE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    LogicValue lMinusOne = (double)-1;
    return E2 * lMinusOne;
}

LogicValue LogicFunction::nativeAbsoluteE1(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    if ((double)E1 >= 0.0)
        return E1;
    else
        return nativeNegativeE1(_channelIndex, DptE1, E1, DptE2, E2, DptOut, iOld);
}

LogicValue LogicFunction::nativeAbsoluteE2(uint8_t _channelIndex, PT_LogicDpt DptE1, LogicValue E1, PT_LogicDpt DptE2, LogicValue E2, PT_LogicDpt *DptOut, LogicValue iOld)
{
    if ((double)E2 >= 0.0)
        return E2;
    else
        return nativeNegativeE2(_channelIndex, DptE1, E1, DptE2, E2, DptOut, iOld);
}

// do not touch after this point

LogicFunction::LogicFunction() {};

LogicFunction::~LogicFunction() {};

LogicValue (*LogicFunction::nativeFunction[NUM_NATIVE_FUNCTIONS])(uint8_t, PT_LogicDpt, LogicValue, PT_LogicDpt, LogicValue, PT_LogicDpt *, LogicValue){
    nativeAdd,
    nativeSubtract,
    nativeMultiply,
    nativeDivide,
    nativeAverage,
    nativeMinimum,
    nativeMaximum,
    nativeModulo,
    nativeAnd,
    nativeOr,
    nativeXor,
    nativeLShift,
    nativeRShift,
    nativeBool2Int,
    nativeSmooth,
    nativeIncrementE1,
    nativeIncrementE2,
    nativeIncrementA,
    nativeDecrementE1,
    nativeDecrementE2,
    nativeDecrementA,
    nativeAddE1,
    nativeAddE2,
    nativeAddE1PlusE2,
    nativeAddE1MinusE2,
    nativeSubtractE1,
    nativeSubtractE2,
    nativeSubtractE1PlusE2,
    nativeSubtractE1MinusE2,
    nativeMinimumA,
    nativeMaximumA,
    nativeLShiftE1,
    nativeLShiftE2,
    nativeLShiftA,
    nativeRShiftE1,
    nativeRShiftE2,
    nativeRShiftA,
    nativePower,
    nativeNegativeE1,
    nativeNegativeE2,
    nativeAbsoluteE1,
    nativeAbsoluteE2};

// dispatcher
LogicValue LogicFunction::callFunction(uint8_t _channelIndex, uint8_t iId, PT_LogicDpt iDptE1, LogicValue iE1, PT_LogicDpt iDptE2, LogicValue iE2, PT_LogicDpt *cDptOut, LogicValue iOld)
{
    if (iId > 0 && iId <= NUM_NATIVE_FUNCTIONS)
    {
        LogicValue lResult = nativeFunction[iId - 1](_channelIndex, iDptE1, iE1, iDptE2, iE2, cDptOut, iOld);
        return lResult;
    }
    else if (iId > 200 && iId <= 230)
    {
        uint8_t lFormulaIndex = iId - 201;
        bool lFormulaActive = knx.paramByte(LOG_UserFormula1Active + lFormulaIndex * (LOG_UserFormula2Active - LOG_UserFormula1Active)) & LOG_UserFormula1ActiveMask;
        if (lFormulaActive)
        {
            // needs to be this way, don't know why
            LogicValue lResult = callUserFormula(lFormulaIndex, iE1, iE2, iOld);
            return lResult;
        }
    }
    return (uint8_t)0;
}

// new user formulas
const uint8_t LogicFunction::sVarsSize = 37;
// bind variables and functions to parser
te_variable LogicFunction::sVars[] = {
    {"e1", &e1},
    {"e2", &e2},
    {"a", &out},
    {"if", (double *)myIf, TE_FUNCTION3},
    {"if2", (double *)myIf2, TE_FUNCTION5},
    {"if3", (double *)myIf3, TE_FUNCTION7},
    {"round", (double *)myRound, TE_FUNCTION2},
    {"rnd", (double *)myRnd, TE_FUNCTION2},
    {"nan", (double *)myNan, TE_FUNCTION0},
    {"b1", (double *)myB1, TE_FUNCTION3},
    {"b2", (double *)myB2, TE_FUNCTION3},
    {"b3", (double *)myB3, TE_FUNCTION3},
    {"b4", (double *)myB4, TE_FUNCTION3},
    {"b5", (double *)myB5, TE_FUNCTION3},
    {"b6", (double *)myB6, TE_FUNCTION3},
    {"b7", (double *)myB7, TE_FUNCTION3},
    {"b8", (double *)myB8, TE_FUNCTION3},
    {"b9", (double *)myB9, TE_FUNCTION3},
    {"b10", (double *)myB10, TE_FUNCTION3},
    {"b11", (double *)myB11, TE_FUNCTION3},
    {"b12", (double *)myB12, TE_FUNCTION3},
    {"b13", (double *)myB13, TE_FUNCTION3},
    {"b14", (double *)myB14, TE_FUNCTION3},
    {"b15", (double *)myB15, TE_FUNCTION3},
    {"b16", (double *)myB16, TE_FUNCTION3},
    {"b17", (double *)myB17, TE_FUNCTION3},
    {"b18", (double *)myB18, TE_FUNCTION3},
    {"b19", (double *)myB19, TE_FUNCTION3},
    {"b20", (double *)myB20, TE_FUNCTION3},
    {"b21", (double *)myB21, TE_FUNCTION3},
    {"b22", (double *)myB22, TE_FUNCTION3},
    {"b23", (double *)myB23, TE_FUNCTION3},
    {"b24", (double *)myB24, TE_FUNCTION3},
    {"b25", (double *)myB25, TE_FUNCTION3},
    {"b26", (double *)myB26, TE_FUNCTION3},
    {"b27", (double *)myB27, TE_FUNCTION3},
    {"b28", (double *)myB28, TE_FUNCTION3},
    {"b29", (double *)myB29, TE_FUNCTION3},
    {"b30", (double *)myB30, TE_FUNCTION3}};

double LogicFunction::myIf(double iCondition, double iTrue, double iFalse)
{
    return iCondition ? iTrue : iFalse;
}

double LogicFunction::myIf2(double iCondition1, double iTrue1, double iCondition2, double iTrue2, double iFalse)
{
    return iCondition1 ? iTrue1 : (iCondition2 ? iTrue2 : iFalse);
}

double LogicFunction::myIf3(double iCondition1, double iTrue1, double iCondition2, double iTrue2, double iCondition3, double iTrue3, double iFalse)
{
    return iCondition1 ? iTrue1 : (iCondition2 ? iTrue2 : (iCondition3 ? iTrue3 : iFalse));
}

double LogicFunction::myRound(double iValue, double iPrecision)
{
    return round(iValue * pow(10, iPrecision)) / pow(10, iPrecision);
}

double LogicFunction::myRnd(double iLower, double iUpper)
{
    if ((long)iUpper >= 0x7FFFFFFF) 
        return iLower;
    return random((long)iLower, (long)iUpper + 1); 
}

double LogicFunction::myNan()
{
    return NAN;
}

// wrapper functions for User Formulas
double LogicFunction::myB1(double iE1, double iE2, double iOut)
{
    return callUserFormula(0, iE1, iE2, iOut);
}

double LogicFunction::myB2(double iE1, double iE2, double iOut)
{
    return callUserFormula(1, iE1, iE2, iOut);
}

double LogicFunction::myB3(double iE1, double iE2, double iOut)
{
    return callUserFormula(2, iE1, iE2, iOut);
}

double LogicFunction::myB4(double iE1, double iE2, double iOut)
{
    return callUserFormula(3, iE1, iE2, iOut);
}

double LogicFunction::myB5(double iE1, double iE2, double iOut)
{
    return callUserFormula(4, iE1, iE2, iOut);
}

double LogicFunction::myB6(double iE1, double iE2, double iOut)
{
    return callUserFormula(5, iE1, iE2, iOut);
}

double LogicFunction::myB7(double iE1, double iE2, double iOut)
{
    return callUserFormula(6, iE1, iE2, iOut);
}

double LogicFunction::myB8(double iE1, double iE2, double iOut)
{
    return callUserFormula(7, iE1, iE2, iOut);
}

double LogicFunction::myB9(double iE1, double iE2, double iOut)
{
    return callUserFormula(8, iE1, iE2, iOut);
}

double LogicFunction::myB10(double iE1, double iE2, double iOut)
{
    return callUserFormula(9, iE1, iE2, iOut);
}

double LogicFunction::myB11(double iE1, double iE2, double iOut)
{
    return callUserFormula(10, iE1, iE2, iOut);
}

double LogicFunction::myB12(double iE1, double iE2, double iOut)
{
    return callUserFormula(11, iE1, iE2, iOut);
}

double LogicFunction::myB13(double iE1, double iE2, double iOut)
{
    return callUserFormula(12, iE1, iE2, iOut);
}

double LogicFunction::myB14(double iE1, double iE2, double iOut)
{
    return callUserFormula(13, iE1, iE2, iOut);
}

double LogicFunction::myB15(double iE1, double iE2, double iOut)
{
    return callUserFormula(14, iE1, iE2, iOut);
}

double LogicFunction::myB16(double iE1, double iE2, double iOut)
{
    return callUserFormula(15, iE1, iE2, iOut);
}

double LogicFunction::myB17(double iE1, double iE2, double iOut)
{
    return callUserFormula(16, iE1, iE2, iOut);
}

double LogicFunction::myB18(double iE1, double iE2, double iOut)
{
    return callUserFormula(17, iE1, iE2, iOut);
}

double LogicFunction::myB19(double iE1, double iE2, double iOut)
{
    return callUserFormula(18, iE1, iE2, iOut);
}

double LogicFunction::myB20(double iE1, double iE2, double iOut)
{
    return callUserFormula(19, iE1, iE2, iOut);
}

double LogicFunction::myB21(double iE1, double iE2, double iOut)
{
    return callUserFormula(20, iE1, iE2, iOut);
}

double LogicFunction::myB22(double iE1, double iE2, double iOut)
{
    return callUserFormula(21, iE1, iE2, iOut);
}

double LogicFunction::myB23(double iE1, double iE2, double iOut)
{
    return callUserFormula(22, iE1, iE2, iOut);
}

double LogicFunction::myB24(double iE1, double iE2, double iOut)
{
    return callUserFormula(23, iE1, iE2, iOut);
}

double LogicFunction::myB25(double iE1, double iE2, double iOut)
{
    return callUserFormula(24, iE1, iE2, iOut);
}

double LogicFunction::myB26(double iE1, double iE2, double iOut)
{
    return callUserFormula(25, iE1, iE2, iOut);
}

double LogicFunction::myB27(double iE1, double iE2, double iOut)
{
    return callUserFormula(26, iE1, iE2, iOut);
}

double LogicFunction::myB28(double iE1, double iE2, double iOut)
{
    return callUserFormula(27, iE1, iE2, iOut);
}

double LogicFunction::myB29(double iE1, double iE2, double iOut)
{
    return callUserFormula(28, iE1, iE2, iOut);
}

double LogicFunction::myB30(double iE1, double iE2, double iOut)
{
    return callUserFormula(29, iE1, iE2, iOut);
}

const std::string LogicFunction::logPrefix(uint8_t iFormulaIndex)
{
    static char lPrefix[20] = {0};
    sprintf(lPrefix, "Logic:<FormulaB%i>", iFormulaIndex + 1);
    return std::string(lPrefix);
}

uint8_t LogicFunction::toLower(const char *iSource, char *iTarget)
{
    uint8_t lCharIndex = 0;
    bool lClearNext = false;
    for (lCharIndex = 0; lCharIndex < 99 && iSource[lCharIndex] > 0; lCharIndex++)
    {
        char lChar = iSource[lCharIndex];
        if (lChar >= 65 && lChar <= 90) lChar += 32; // convert to lowercase
        // get rid of unreplaced '\n' in formula
        if (lClearNext)
        {
            lChar = 32;
            lClearNext = false;
        }
        if (lChar == 92 && iSource[lCharIndex + 1] == 110)
        {
            lChar = 32;
            lClearNext = true;
        }
        iTarget[lCharIndex] = lChar;
    }
    iTarget[lCharIndex] = 0; // ensure zero termination
    return lCharIndex;
}

LogicValue LogicFunction::callUserFormula(uint8_t iFormulaIndex, double iE1, double iE2, double iOut)
{
    double lResult = 0;
    bool lFailed = false;

    // Preserve caller context for nested B-formula calls.
    double lPreviousE1 = e1;
    double lPreviousE2 = e2;
    double lPreviousOut = out;

    sRecursionCounter++;
    if (sRecursionCounter <= NUM_RECURSION_DEPTH)
    {
        const char *lEtsFormula = (char *)knx.paramData(LOG_UserFormula1 + iFormulaIndex * (LOG_UserFormula2 - LOG_UserFormula1));
        logDebug(logPrefix(iFormulaIndex), "Evaluating: %.99s", lEtsFormula);
        // char lFormula[100] = {0};
        toLower(lEtsFormula, sFormulaBuffer);

        // parse formula
        int lError;
        te_expr *lParsedFormula = te_compile(sFormulaBuffer, sVars, sVarsSize, &lError);

        if (lParsedFormula)
        {
            e1 = iE1;
            e2 = iE2;
            out = iOut;
            logDebug(logPrefix(iFormulaIndex), "Values: e1=%f, e2=%f, a=%f", e1, e2, out);
            lResult = te_eval(lParsedFormula);
            logDebug(logPrefix(iFormulaIndex), "Result: %f", lResult);
            te_free(lParsedFormula);
        }
        else
        {
            /* Show the user where the error is at. */
            // const char lErrorChar = lEtsFormula[lError - 1];
            // lFormula[lError - 1] = 0;
            logDebug(logPrefix(iFormulaIndex), "Error near '%c': %.*s -->%c<-- %.*s", lEtsFormula[lError - 1], lError - 1, lEtsFormula, lEtsFormula[lError - 1], 99 - lError, lEtsFormula + lError);
            lFailed = true;
        }
    }
    else
    {
        logDebug(logPrefix(iFormulaIndex), "Error: Recursion limit (%i calls) reached", NUM_RECURSION_DEPTH);
        lFailed = true;
    }
    sRecursionCounter--;

    // Restore the variables of the caller after a nested formula call.
    e1 = lPreviousE1;
    e2 = lPreviousE2;
    out = lPreviousOut;

    if (lFailed)
        return LogicValue(NAN);
    else
        return LogicValue(lResult);
}

void LogicFunction::processFunctionPropertyFormula(std::string iLogPrefix, bool iCalculate, uint8_t *iData, uint8_t *eResultData, uint8_t &eResultLength)
{
    uint8_t lFormulaLength = iData[2];
    const char *lEtsFormula = (char *)iData + 3;
    logIndentUp();
    logDebug(iLogPrefix, "Formula: %s", lEtsFormula);
    uint8_t lReceivedLen = toLower(lEtsFormula, sFormulaBuffer);
    if (lReceivedLen == lFormulaLength)
    {
        // parse formula
        int lError;
        te_expr *lParsedFormula = te_compile(sFormulaBuffer, sVars, sVarsSize, &lError);
        if (lParsedFormula)
        {
            eResultData[0] = 0;
            if (iCalculate)
            {
                e1 = 0;
                e2 = 0;
                out = 0;
                double lResult = te_eval(lParsedFormula);
                logDebug(iLogPrefix, "Result: %f", lResult);
                eResultLength = sprintf((char *)eResultData + 1, "%f", lResult) + 1;
                for (eResultLength--; eResultLength >= 1 && eResultData[eResultLength] == '0'; eResultLength--)
                    eResultData[eResultLength] = 0;
                if (eResultData[eResultLength] == '.')
                    eResultData[eResultLength] = 0;
                else
                    eResultLength++;
            }
            else
            {
                logDebug(iLogPrefix, "Formula is OK!");
                eResultLength = 1;
            }
            te_free(lParsedFormula);
        }
        else
        {
            eResultData[0] = lError;
            eResultLength = 1;
            logDebug(iLogPrefix, "Error near '%c' at position %d!", lEtsFormula[lError - 1], lError);
        }
    }
    else
    {
        eResultData[0] = -1;
        eResultLength = 1;
        logDebug(iLogPrefix, "Received formula is too short - most probably an APDU problem. Got %i, should be %i", lReceivedLen, lFormulaLength);
    }
    logIndentDown();
}

void LogicFunction::handleFunctionPropertyCheckFormula(uint8_t *iData, uint8_t *eResultData, uint8_t &eResultLength)
{
    uint8_t lFormulaIndex = iData[1];
    logDebug(logPrefix(lFormulaIndex), "Function property: Check user formula");
    processFunctionPropertyFormula(logPrefix(lFormulaIndex), false, iData, eResultData, eResultLength);
}

void LogicFunction::handleFunctionPropertyTestFormula(uint8_t *iData, uint8_t *eResultData, uint8_t &eResultLength)
{
    logDebug("Logic<FormulaTest>", "Function property: Test user formula");
    processFunctionPropertyFormula("Logic<FormulaTest>", true, iData, eResultData, eResultLength);
}