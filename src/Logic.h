#pragma once
#include "LogicChannel.h"
#include "OpenKNX.h"
#include "Timer.h"
#include "TimerRestore.h"

#define USERDATA_MAGIC_OFFSET 0 // start of magic word in flash
#define USERDATA_DPT_OFFSET 4   // start of DPT storage in flash
#define USERDATA_KO_OFFSET 204  // start of KO values in flash (2 KO per channel, 4 Byte per KO)

// Watchdog reset causes
#define WDT_RCAUSE_SYSTEM 6 // reset by system itself
#define WDT_RCAUSE_WDT 5    // reset by watchdog
#define WDT_RCAUSE_EXT 4    // reset by reset signal
#define WDT_RCAUSE_POR 0    // power on reset

// typedef void (*loopCallback)(void *iThis);
// struct sLoopCallbackParams {
//     loopCallback callback;
//     void *instance;
// };

struct sKoLookup
{
    uint16_t koNumber;
    uint8_t channelIndex;
    uint8_t ioIndex;
};

class Logic : public OpenKNX::Module
{
  public:
    Logic();
    ~Logic();

    // instance
    void addKoLookup(uint16_t iKoNumber, uint8_t iChannelId, uint8_t iIOIndex);
    bool getKoLookup(uint16_t iKoNumber, sKoLookup **iKoLookup = nullptr);
    LogicChannel *getChannel(uint8_t iChannelId);

    void processAllInternalInputs(LogicChannel *iChannel, bool iValue);
    void processAfterStartupDelay();
    void processInputKo(GroupObject &iKo);
    void showHelp() override;
    bool processCommand(const std::string cmd, bool debugKo) override;
    void initLoadCounter(bool iAll);
    bool sendReadRequest(GroupObject &ko);

    void debug();
    void setup();
    void loop();

    bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
    void readFlash(const uint8_t *iBuffer, const uint16_t iSize) override;
    void writeFlash() override;
    uint16_t flashSize() override;
    const std::string name() override;
    const std::string version() override;

    // TODO check removal from public api!
    tm &Now();
    bool timeValid();
    char gBuffer[14] = {0};

  private:
    static const uint8_t helpCommandCount = 10;

    static uint8_t sMagicWord[];
    static Timer &sTimer;
    static TimerRestore &sTimerRestore;
    static const char* helpCommands[];

    LogicChannel *mChannel[LOG_ChannelCount];
    uint8_t mNumChannels; // Number of channels defined in knxprod
    uint8_t mChannelIterator = 0;

    // we need a lookup for external KO
    static const uint16_t cCountKoLookups = LOG_ChannelCount * 3;
    sKoLookup mKoLookup[cCountKoLookups]; // max 3*4*100 = 1200 Byte, too much?
    uint16_t mNumKoLookups = 0;
    uint32_t readRequestDelay = 0;

    void prepareChannels();

    void processTimerRestore();
};

extern Logic openknxLogic;