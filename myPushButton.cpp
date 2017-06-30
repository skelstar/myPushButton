#include "Arduino.h"
#include "EventManager.h"
#include "myPushButton.h"

EventManager evM;
#define EVENT_CODE EventManager::kEventUser9

#define DOUBLETAP_MS_HIGH   1000
#define DOUBLETAP_MS_LOW    200

long lastButtonReleaseTime = 0;

// declarations
bool buttonReleaseInDoubletapWindow();

myPushButton::myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint8_t lowState, EventListener listenerCallback) {

    _pin = pin;
    _pullUp = pullUp;
    _lowState = lowState;
    _heldDurationMillis = heldDurationMs;

    _state = ST_NOT_HELD;

    if (listenerCallback != NULL) {
        evM.addListener(EVENT_CODE, listenerCallback);
    }

    init2();
}

void myPushButton::serviceEvents() {
    bool buttonPressed = isPressed();

    if (_state == ST_NOT_HELD) {
        if (buttonPressed) {
            _state = EV_BUTTON_PRESSED;
        }

    } else if (_state == EV_BUTTON_PRESSED ) {
        _heldBeginMillis = millis();
        evM.queueEvent(EVENT_CODE, _state);
        _state = ST_WAIT_FOR_HELD_TIME;

    } else if (_state == ST_WAIT_FOR_HELD_TIME) {
        if (buttonPressed && isHeldForLongEnough()) {
            _state = EV_HELD_FOR_LONG_ENOUGH;
        } else if (!buttonPressed) {
            _state = EV_RELEASED;
        }

    } else if (_state == EV_HELD_FOR_LONG_ENOUGH) {
        evM.queueEvent(EVENT_CODE, _state);
        _state = ST_WAITING_FOR_RELEASE_FROM_HELD_TIME;
        lastButtonReleaseTime = 0;

    } else if (_state == ST_WAITING_FOR_RELEASE) {
        if (!buttonPressed) {
            _state = EV_RELEASED;
        }
    
    } else if (_state == ST_WAITING_FOR_RELEASE_FROM_HELD_TIME) {
        if (!buttonPressed) {
            _state = EV_RELEASED_FROM_HELD_TIME;
        }
    } 

    if (_state == EV_RELEASED) {
        evM.queueEvent(EVENT_CODE, _state);
        if (buttonReleaseInDoubletapWindow()) {
            evM.queueEvent(EVENT_CODE, EV_DOUBLETAP);
        	lastButtonReleaseTime = 0;
        } else {
            lastButtonReleaseTime = millis();
            _state = ST_NOT_HELD;
        }
    } else if (_state == EV_RELEASED_FROM_HELD_TIME) {
        evM.queueEvent(EVENT_CODE, _state);
        lastButtonReleaseTime = millis();
        _state = ST_NOT_HELD;
    }

    evM.processEvent();
}

void myPushButton::init2() {
    if (_pullUp) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

    delayMicroseconds(5);
}

bool myPushButton::isPressed() {
    init2();
    return digitalRead(_pin) != _lowState;
}

bool myPushButton::isHeldForLongEnough() {
    return long(_heldBeginMillis + _heldDurationMillis) <= millis();
}

bool buttonReleaseInDoubletapWindow() {
    long gap = millis() - lastButtonReleaseTime;
    return gap > DOUBLETAP_MS_LOW && gap < DOUBLETAP_MS_HIGH;
}