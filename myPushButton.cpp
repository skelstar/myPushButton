#include "Arduino.h"
#include "myPushButton.h"

#define DOUBLETAP_MS_HIGH   1000
#define DOUBLETAP_MS_LOW    200
#define EVENT_CODE		0

long lastButtonReleaseTime = 0;

long _heldBeginMillis;
int _heldSecondsCounter;

// declarations
bool buttonReleaseInDoubletapWindow();

//--------------------------------------------------------------------------------

myPushButton::myPushButton(uint8_t pin, 
						   bool pullUp, 
						   uint8_t offState, 
						   EventListener listenerCallback) {
    _pin = pin;
    _pullUp = pullUp;
    _offState = offState;
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
				_state = EV_BUTTON_PRESSED;
			}
			break;

		case EV_BUTTON_PRESSED:
			//Serial.println("*** EV_BUTTON_PRESSED");
			_heldBeginMillis = millis();
			_heldSecondsCounter = 0;
			_listenerCallback(EV_BUTTON_PRESSED, _pin, 0);
			_state = ST_WAIT_FOR_RELEASE;
			break;

		case ST_WAIT_FOR_RELEASE:
			if (buttonPressed == false) {
				_state = EV_RELEASED;
			}
			else {
				long heldDownDuration = millis() - _heldBeginMillis;
				if ((_heldSecondsCounter+1)*1000 < heldDownDuration) {
					_heldSecondsCounter++;
					//Serial.print("EV_HELD_SECONDS_"); Serial.println(_heldSecondsCounter);
					_listenerCallback(EV_HELD_SECONDS, _pin, _heldSecondsCounter);
				}
    		}
    		break;

		case EV_RELEASED :
			//Serial.println("EV_RELEASED");
			_listenerCallback(EV_RELEASED, _pin, _heldSecondsCounter);
	        if (buttonReleaseInDoubletapWindow()) {
				//Serial.println("*** EV_DOUBLETAP");
				_listenerCallback(EV_DOUBLETAP, _pin, _heldSecondsCounter);
				lastButtonReleaseTime = 0;
	        } 
	        else {
	            lastButtonReleaseTime = millis();
	        }
			_state = ST_NOT_HELD;
			break;
	}
}

void myPushButton::init() {
    if (_pullUp) {
        pinMode(_pin, INPUT_PULLUP);
        // digitalWrite(_pin, HIGH);
    } else {
        pinMode(_pin, INPUT);
    }

    delayMicroseconds(5);
}

bool myPushButton::isPressed() {
    init();
    return digitalRead(_pin) != _offState;
}

bool buttonReleaseInDoubletapWindow() {
    long gap = millis() - lastButtonReleaseTime;
    return gap > DOUBLETAP_MS_LOW && gap < DOUBLETAP_MS_HIGH;
}