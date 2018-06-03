#include <myPushButton.h>
//simple.ino

void listener_Button(int eventCode, int eventPin, int eventParam);

#define PULLUP		true
#define OFFSTATE	LOW

myPushButton button(A1, PULLUP, OFFSTATE, listener_Button);

void listener_Button(int eventCode, int eventPin, int eventParam) {
    
	switch (eventCode) {
		case button.EV_BUTTON_PRESSED:
			sendEventAndPin("EV_BUTTON_PRESSED", eventPin, NULL);
			break;
		case button.EV_RELEASED:
			sendEventAndPin("EV_RELEASED", eventPin, eventParam);
			break;
		case button.EV_DOUBLETAP:
			sendEventAndPin("EV_DOUBLETAP", eventPin, NULL);
			break;
		case button.EV_HELD_SECONDS:
			sendEventAndPin("EV_HELD_SECONDS", eventPin, eventParam);
			break;
    }
}

void sendEventAndPin(char* event, int pin, int params) {
	char payload1[30];
	char pinbuff[3];

	strcpy(payload1, event);
	itoa(pin, pinbuff, 10);
	strcat(payload1, "|");
	strcat(payload1, pinbuff);
	strcat(payload1, "~");
	itoa(params, pinbuff, 10);
	strcat(payload1, pinbuff);
	puts(payload1);
	Serial.println(payload1);
	//wifiHelper.mqttPublish(TOPIC_EVENT, payload1);
}

void setup() {

    Serial.begin(9600);
    delay(200);
    Serial.println("Booting");
}

void loop() {
	button.serviceEvents();
    Serial.println(button.isPressed());
    delay(200);
}
