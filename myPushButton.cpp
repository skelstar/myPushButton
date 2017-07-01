#include "Arduino.h"
#include "myPushButton.h"

#define DOUBLETAP_MS_HIGH   1000
#define DOUBLETAP_MS_LOW    200
#define EVENT_CODE		0

long lastButtonReleaseTime = 0;

long _heldBeginMillis;
long _heldDurationMillis;
long _heldLongDurationMillis;
int heldSecondsCounter;

// declarations
bool buttonReleaseInDoubletapWindow();
void checkHeldDownSeconds();
bool isHeldForLongEnough();
bool isHeldForLongLongEnough();

//--------------------------------------------------------------------------------

myPushButton::myPushButton(uint8_t pin, 
						   bool pullUp, 
						   uint16_t heldDurationMs, 
						   uint16_t heldLongDurationMs, 
						   uint8_t offState, 
						   EventListener listenerCallback) {

    _pin = pin;
    _pullUp = pullUp;
    _offState = offState;
    _heldDurationMillis = heldDurationMs;
    _heldLongDurationMillis = heldLongDurationMs;
    _listenerCallback = listenerCallback;

    _state = ST_NOT_HELD;

    init();
}

void myPushButton::serviceEvents() {

    bool buttonPressed = isPressed();
    bool buttonReleased = !isPressed();

	switch (_state) {

		case ST_NOT_HELD:
			if (buttonPressed) {
				//Serial.println("*** EV_BUTTON_PRESSED");
				_state = EV_BUTTON_PRESSED;
			}
			break;

		case EV_BUTTON_PRESSED:
			_heldBeginMillis = millis();
			heldSecondsCounter = 1;
			_listenerCallback(EVENT_CODE, EV_BUTTON_PRESSED);
			// Serial.println("ST_WAIT_FOR_HELD_TIME");
			_state = ST_WAIT_FOR_HELD_TIME;
			break;

		case ST_WAIT_FOR_HELD_TIME:
			if (buttonPressed && isHeldForLongEnough()) {
				// Serial.println("*** EV_HELD_FOR_LONG_ENOUGH");
				_state = EV_HELD_FOR_LONG_ENOUGH;
			}
			else if (buttonReleased) {
				// Serial.println("EV_RELEASED");
				_state = EV_RELEASED;
			}
			break;

		case EV_HELD_FOR_LONG_ENOUGH:
			_listenerCallback(EVENT_CODE, EV_HELD_FOR_LONG_ENOUGH);
			// Serial.println("ST_WAITING_FOR_LONG_HELD_TIME");
			_state = ST_WAITING_FOR_LONG_HELD_TIME;
			lastButtonReleaseTime = 0;
			break;

		case ST_WAITING_FOR_LONG_HELD_TIME:
			if (buttonPressed && isHeldForLongLongEnough()) {
				// Serial.println("*** EV_HELD_FOR_LONG_LONG_ENOUGH");
				_state = EV_HELD_FOR_LONG_LONG_ENOUGH;
			}
			else if (buttonReleased) {
				// Serial.println("EV_RELEASED_FROM_HELD_TIME");
				_state = EV_RELEASED_FROM_HELD_TIME;
			}
			break;

		case EV_HELD_FOR_LONG_LONG_ENOUGH:
			_listenerCallback(EVENT_CODE, EV_HELD_FOR_LONG_LONG_ENOUGH);
			// Serial.println("ST_WAITING_FOR_RELEASE_FROM_HELD_LONG_TIME");
			_state = ST_WAITING_FOR_RELEASE_FROM_HELD_LONG_TIME;
			lastButtonReleaseTime = 0;
			break;

		case ST_WAITING_FOR_RELEASE_FROM_HELD_LONG_TIME:
			if (buttonReleased) {
				// Serial.println("EV_RELEASED_FROM_LONG_HELD_TIME");
				_state = EV_RELEASED_FROM_LONG_HELD_TIME;
			}
			break;
	}

    if (_state == EV_RELEASED) {
        if (buttonReleaseInDoubletapWindow()) {
            _listenerCallback(EVENT_CODE, EV_DOUBLETAP);
			// Serial.println("*** EV_DOUBLETAP");
        	lastButtonReleaseTime = 0;
        } else {
            lastButtonReleaseTime = millis();
			// Serial.println("ST_NOT_HELD");
			_listenerCallback(EVENT_CODE, EV_RELEASED);
            _state = ST_NOT_HELD;
        }
    } 
    else if (_state == EV_RELEASED_FROM_HELD_TIME ||
		   	 _state == EV_RELEASED_FROM_LONG_HELD_TIME) {
		_listenerCallback(EVENT_CODE, _state);
		// Serial.println("ST_NOT_HELD");
		_state = ST_NOT_HELD;
		lastButtonReleaseTime = millis();
    }

    // EV_HELD_SECONDS
    if (_state != ST_NOT_HELD && buttonPressed) {
		long heldDownDuration = millis() - _heldBeginMillis;
		if ((heldSecondsCounter)*1000 < heldDownDuration) {
			if (heldSecondsCounter > 1) {
				_listenerCallback(heldSecondsCounter-1, EV_HELD_SECONDS);
				//Serial.print("EV_HELD_SECONDS_"); Serial.println(heldSecondsCounter);
			}
			heldSecondsCounter++;
		}
    }
}

void myPushButton::init() {
    if (_pullUp) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

    delayMicroseconds(5);
}

bool myPushButton::isPressed() {
    init();
    return digitalRead(_pin) != _offState;
}

void checkHeldDownSeconds() {
}

bool isHeldForLongEnough() {
    return long(_heldBeginMillis + _heldDurationMillis) <= millis();
}

bool isHeldForLongLongEnough() {
	return long(_heldBeginMillis + _heldLongDurationMillis) <= millis();
}

bool buttonReleaseInDoubletapWindow() {
    long gap = millis() - lastButtonReleaseTime;
    return gap > DOUBLETAP_MS_LOW && gap < DOUBLETAP_MS_HIGH;
}