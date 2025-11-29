#pragma once
#include "OpenKNX/Led/Base.h"
#include "Logic.h"

class LogicLed : public OpenKNX::Led::Base
{
    private:
        LogicChannel* mLogicChannel = nullptr;
        uint8_t mIOIndex = 0;

    protected:
        void writeLed(uint8_t brightness) override {};

    public:
        LogicLed(LogicChannel *iChannel, uint8_t iIOIndex);
        
        void loop() override {};

        void init() override {};
 
        void powerSave(bool active = true) override {}

        void errorCode(uint8_t code = 0) override {}

        void on(bool active = true) override;
        
        void forceOn(bool active = true) override { on(active); };

        void pulsing(uint16_t duration = OPENKNX_LEDEFFECT_PULSE_FREQ) override { on(); }

        void blinking(uint16_t frequency = OPENKNX_LEDEFFECT_BLINK_FREQ) override { on(); }

        void flash(uint16_t frequency = OPENKNX_LEDEFFECT_FLASH_DURATION) override { on(); }

        void activity(uint32_t &lastActivity, bool inverted = false) override { on(inverted); };

        std::string logPrefix() override { return "LogicLed"; }

    };