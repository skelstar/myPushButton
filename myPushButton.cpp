

#include "Arduino.h"
#include "EventManager.h"
#include "myPushButton.h"

EventManager evM;
#define EVENT_CODE EventManager::kEventUser9

#define DEBOUNCE_DELAY_MS   10

myPushButton::myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint8_t lowState, EventListener listenerCallback) {

    _pin = pin;
    _pullUp = pullUp;
    _lowState = lowState;
    _heldDurationMillis = heldDurationMs;

    _state = ST_NOT_HELD;

    evM.addListener(EVENT_CODE, listenerCallback);

    init2();
}

bool myPushButton::singleButtonPush() {
    return true;    // _state == HELD_FOR_LESS_THAN_HELD_TIME
}

void myPushButton::serviceEvents() {
    bool buttonPressed = isPressed();

    switch (_state) {

        case ST_NOT_HELD:
            if (buttonPressed) {
                _state = EV_BUTTON_PRESSED;
            }
            break;
        case EV_BUTTON_PRESSED:
            _heldBeginMillis = millis();
            evM.queueEvent(EVENT_CODE, _state);
            delay(DEBOUNCE_DELAY_MS);
            _state = ST_WAIT_FOR_HELD_TIME;
            break;
        case ST_WAIT_FOR_HELD_TIME:
            if (buttonPressed && isHeldForLongEnough()) {
                _state = EV_HELD_FOR_LONG_ENOUGH;
            } else if (!buttonPressed) {
                _state = EV_RELEASED;
            }
            break;
        case EV_HELD_FOR_LONG_ENOUGH:
            evM.queueEvent(EVENT_CODE, _state);
            delay(DEBOUNCE_DELAY_MS);
            _state = ST_WAITING_FOR_RELEASE;
            break;
        case ST_WAITING_FOR_RELEASE:
            if (!isPressed()) {
                _state = EV_RELEASED;
            }
            break;
        case EV_RELEASED:
            evM.queueEvent(EVENT_CODE, _state);
            delay(DEBOUNCE_DELAY_MS);
            _state = ST_NOT_HELD;
            break;
    }    
    evM.processEvent();
}

void myPushButton::init2() 
{
    if (_pullUp) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

    delayMicroseconds(5);
}

bool myPushButton::isPressed() 
{
    return digitalRead(_pin) != _lowState;
}

bool myPushButton::isHeldForLongEnough() 
{
    return uint16_t(_heldBeginMillis + _heldDurationMillis) <= millis();
}