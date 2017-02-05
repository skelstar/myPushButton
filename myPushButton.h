#ifndef myPushButton_h
#define myPushButton_h

#include <Arduino.h>
#include <EventManager.h>

class myPushButton 
{
    public:
        enum StateCode {
            ST_NOT_HELD,
            EV_BUTTON_PRESSED,
            ST_WAIT_FOR_HELD_TIME,
            EV_HELD_FOR_LONG_ENOUGH,
            ST_WAITING_FOR_RELEASE,
            ST_WAITING_FOR_RELEASE_FROM_HELD_TIME,
            EV_RELEASED,
            EV_RELEASED_FROM_HELD_TIME,
            EV_DOUBLETAP
        };

        typedef void ( *EventListener )( int eventCode, int eventParam );
        myPushButton(uint8_t pin, bool pullUp, uint16_t heldDurationMs, uint8_t lowState, EventListener listenerCallback);
        bool isPressed();
        void serviceEvents();
        bool singleButtonPush();
        bool isHeld();
        bool isReleased();
    private:
        EventManager evM;
        uint8_t _pin;
        bool _pullUp;
        uint8_t _state;
        uint8_t _lowState;
        long _heldDurationMillis;
        long _heldBeginMillis;

        bool isHeldForLongEnough();
        void init2();
};

#endif
