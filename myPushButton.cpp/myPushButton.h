
#pragma once

#include <Arduino.h>

class myPushButton {

    public:
        myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs);
        bool buttonIsHeld();
    private:
        uint8_t _pin;
        bool _pullUp;
        uint8_t _state;
        uint16_t _heldDurationMillis;
        uint16_t _heldBeginMillis;
        int getButton();
        void init2();
}

