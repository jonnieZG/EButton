#include "Arduino.h"

// ------- Specify not required features before including the EButton.h --------
#define EBUTTON_SUPPORT_TRANSITION_DISABLED
#define EBUTTON_SUPPORT_EACH_CLICK_DISABLED
//#define EBUTTON_SUPPORT_DONE_CLICKING_DISABLED
#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_START_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_DURING_DISABLED

#include "EButton.h"

EButton button(2);

void countClicks(EButton &btn) {
	Serial.print("\nYou've managed to click ");
	Serial.print(btn.getClicks());
	Serial.println(" time(s)!");
}
void setup() {
	Serial.begin(115200);
	button.attachDoneClicking(countClicks);
	Serial.println("\nClick as fast as you can!");
}

void loop() {
	button.tick();
}
