
#include "myPushButton.h"

myPushButton :: myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint8_t lowState) {

    _pin = pin;
    _pullUp = pullUp;
    _lowState = lowState;
    _heldDurationMillis = heldDurationMs;
}

/*
 * state 0: button not held
 * state 1: button held but less than heldTime
 * state 2: button held for longer than heldTime
 */

 #define NOT_HELD   0
 #define HELD_FOR_LESS_THAN_HELD_TIME
 #define HELD_FOR_MORE_THAN_HELD_TIME

bool myPushButtonStateMachine::buttonIsHeld() {

    bool buttonPressed = isPressed();
    
    switch (state) {

        case NOT_HELD:
            if (buttonPressed) {
                state = HELD_FOR_LESS_THAN_HELD_TIME;
                heldBeginMs = millis();
            }
            break;
        case HELD_FOR_LESS_THAN_HELD_TIME:
            if (buttonPressed && isHeldForLongEnough) {
                state = HELD_FOR_MORE_THAN_HELD_TIME;
                return true;
            } else if (!buttonPressed) {
                state = NOT_HELD;
            }
            break;
        case HELD_FOR_MORE_THAN_HELD_TIME:
            if (buttonPressed) {
                return true;
            } else {
                state = NOT_HELD;
            }
            break;
    }
    return false;
}

void init2() {
    if (_pullUp) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

    delayMicroseconds(5);
}

bool myPushButton::isPressed() {
    init();
    return digitalRead(_pin) != _defaultState;
}

bool myPushButton::isHeldForLongEnough() {
    return uint16_t(heldBeginMs + heldDurationMillis) <= millis();
}


