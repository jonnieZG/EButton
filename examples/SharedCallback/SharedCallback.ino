#include "EButton.h"

EButton buttons[] = {
  {6, true, 1}, {4, true, 2}, {2, true, 3},
  {7, true, 4}, {5, true, 5}, {3, true, 6}};


void sharedClick(EButton& btn) {
  Serial.println(btn.getID());
}


void setup() {
  Serial.begin(115200);

  for (EButton& btn: buttons) {
    btn.attachSingleClick(sharedClick);
  }
}

void loop() {
  for (EButton& btn: buttons) {
    btn.tick();
  }
}
