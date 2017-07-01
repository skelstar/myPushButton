#ifndef myPushButton_h
#define myPushButton_h

#include <Arduino.h>

class myPushButton 
{
    public:
        enum StateCode {
			ST_NOT_HELD,
			EV_BUTTON_PRESSED,
			ST_WAIT_FOR_HELD_TIME,
			EV_HELD_FOR_LONG_ENOUGH,
			EV_HELD_FOR_LONG_LONG_ENOUGH,
			ST_WAITING_FOR_LONG_HELD_TIME,
			ST_WAITING_FOR_RELEASE_FROM_HELD_TIME,
			ST_WAITING_FOR_RELEASE_FROM_HELD_LONG_TIME,
			EV_RELEASED,
			EV_RELEASED_FROM_HELD_TIME,
			EV_RELEASED_FROM_LONG_HELD_TIME,
			EV_RELEASED_SECONDS,
			EV_HELD_SECONDS,
			EV_DOUBLETAP
        };

        typedef void ( *EventListener )( int eventCode, int eventParam );

        myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint16_t heldLongDurationMs, uint8_t offState, EventListener listenerCallback);
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
