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
			EV_SPECFIC_TIME_REACHED,
			EV_RELEASED,
			EV_HELD_SECONDS,
			EV_DOUBLETAP
		};

		typedef void ( *EventListener )( int eventCode, int eventPin, int eventParam );
		//typedef void ( *EventListenerSpecfic )( int eventCode, int eventPin, int eventParam );

        myPushButton(uint8_t pin, bool pullUp, uint8_t offState, EventListener listenerCallback, uint8_t specificEventMillis);
        myPushButton(uint8_t pin, bool pullUp, uint8_t offState, EventListener listenerCallback);
        bool isPressed();
        void serviceEvents();
        bool pressedForNumMs(int numMillis);

    private:
        uint8_t _pin;
        bool _pullUp;
        uint8_t _state;
		uint8_t _offState;
		uint8_t _specificEventMillis = -1;
		bool _specificTimeReached = false;

        EventListener _listenerCallback;
        //EventListenerSpecfic _listenerSpecificCallback;

        void init();
};

#endif
