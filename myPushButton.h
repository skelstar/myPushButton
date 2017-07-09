#ifndef myPushButton_h
#define myPushButton_h

#include <Arduino.h>

class myPushButton 
{
    public:
        enum StateCode {
			ST_NOT_HELD,
			EV_BUTTON_PRESSED,
			ST_WAIT_FOR_RELEASE,
			EV_RELEASED,
			EV_HELD_SECONDS,
			EV_DOUBLETAP
        };

        typedef void ( *EventListener )( int eventCode, int eventPin, int eventParam );

        myPushButton(uint8_t pin, bool pullUp, uint8_t offState, EventListener listenerCallback);
        bool isPressed();
        void serviceEvents();

    private:
        uint8_t _pin;
        bool _pullUp;
        uint8_t _state;
		uint8_t _offState;

        EventListener _listenerCallback;

        void init();
};

#endif
