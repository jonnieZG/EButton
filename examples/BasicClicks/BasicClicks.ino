#include "Arduino.h"

// ------- Specify not required features before including the EButton.h --------
#define EBUTTON_SUPPORT_TRANSITION_DISABLED
#define EBUTTON_SUPPORT_EACH_CLICK_DISABLED
#define EBUTTON_SUPPORT_DONE_CLICKING_DISABLED
//#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_START_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_DURING_DISABLED

#include "EButton.h"

EButton button(2);

void singleClick(EButton &btn) {
	Serial.println("We have a click!");
}

void doubleClick(EButton &btn) {
	Serial.println("We have a double click!");
}

void setup() {
	Serial.begin(115200);
	button.attachSingleClick(singleClick);
	button.attachDoubleClick(doubleClick);

	Serial.println("\nClick or double-click!");
}

void loop() {
	button.tick();
}
