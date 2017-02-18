#include "Arduino.h"
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
