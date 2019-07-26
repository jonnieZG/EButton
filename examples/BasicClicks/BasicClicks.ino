#include "Arduino.h"

// ------- Specify required features before including the EButton.h --------
#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS

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
