# EButton - Single Button Driver

## Why This One?
I wrote this library in need of a reliable, compact driver for controlling a single key in my **`Arduino`** projects, with debouncing
being properly handled, and fine grained, well defined events. Besides that, I wanted a library that I would be able to easily strip of
unneeded features, making its memory footprint as small as possible.

## Quick Start
1. Include the library. *(doh!)*
2. Instantiate the **`EButton`** object, specfying a pin (connected to one lead of the button, while its other lead is connected to GND.
3. Write handler method(s) and attach them to the class.
4. Keep executing the object's `tick()` method in a loop. It does all the magic - reads button state changes and triggers appropriate
   events when detected.

## Clicks and Long-Presses
The class defines two major event types - a **Click** and a **Long-Press**:
* **Click** - an event when the button is released, and it was not in a **Long-Press** state;
* **Long-Press** - when the button is kept pressed at least for a time specified by parameter called `longPressTime`;

## Counting Clicks
It is important to define how long the class will be counting clicks, to tell apart if a button was clicked twice, or if there
was a double-click (or even a 5-click if you're fast enough).

The parameter called `clickTime` is a value in milliseconds, defining how long will the driver wait since the last click, before
wrapping up the count. It does *not* mean that all the clicks have to be performed in that period, but it does specify the maxium
allowed distance between two adjacent clicks.

There are 3 handler slots available for defining custom methods that act on a given number of clicks:
* `singleClickMethod` - Called if there was a single click;
* `doubleClickMethod` - Called if there was a double click;
* `anyClickMethod` - Called when clicks counting is done. Use `getClicks()` to get the clicks count.

## Events
Events are listed in their order of appearence:

1. `TRANSITION`- triggered each time the button state changes from pressed to released, or back;
2. `EACH_CLICK` - triggered each time the key is released, unless it was in `LONG_PRESSED` state;
3. `ANY_CLICK` - triggered after all the clicks have been counted (use `getClicks()` to get the clicks count);
4. `SINGLE_CLICK` - triggered when there was exactly one click;
5. `DOUBLE_CLICK` - triggered when there were exactly two clicks;
6. `LONG_PRESS_START`  triggered once, at the beginning of a long press (after a `TRANSITION` to pressed);
7. `DURING_LONG_PRESS` - triggered on each tick() while in `LONG_PRESSED` state;
8. `LONG_PRESS_END` - triggered once, at the end of a long press (after a `TRANSITION` to released).

Generally, in most of cases it will be enough to handle a SINGLE_CLICK, but that is up to you.

> **NOTE:** It is important to stress the difference between `EACH_CLICK` and `ANY_CLICK`: the first one is called **each time**
> the key is released (unless it was a long-press), while `ANY_CLICK` is called **once**, at the end of clicks counting.

## Handler Methods
For each event there is a slot where you can slip your own methods that is triggered when a corresponding event is detected.

A handler method can be any method returning `void` and accepting one `EButton&` parameter. 
```C++
void anyClick(EButton &btn) {
   Serial.print("Counted clicks: ");
   Serial.println(btn.getClicks());
}
```
All handler methods are optional, and initially set to `NULL`.

## Cutting Down Memory Footprint
If the memory becomes an issue in your project, you can easily decrease the driver's footprint by disabling the unneeded events.
To disable a feature, just comment out its corresponding `#define` entry in the driver's header file:
```C++
#define EBUTTON_SUPPORT_TRANSITION
#define EBUTTON_SUPPORT_EACH_CLICK
#define EBUTTON_SUPPORT_ANY_CLICK
#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS
#define EBUTTON_SUPPORT_LONG_PRESS
```
> **NOTE:** If you disable `EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS`, then you can use the `ANY_CLICK` event to process
> single, double, and any other number of clicks. Just use `getClicks()` to get the final clicks count.

## Debouncing
Due to imperfections of electrical contacts, in most buttons and switches, the state does not just go from one state to another and
stays there. Instead, there is always a certain period (depending on the actual switch, between a few and several dozens of milliseconds),
in which the contact oscillates between a *high* and a *low* state. 

Therefore, the easiest way to get a reliable reading, is to wait after the first detected change of state for a certain period of time
(`debounceTime`), before you get the second reading. Debouncing alway occurs at a change of state, and should not normally occur in
a stable state.

The driver uses a default debounce value defined with the `EBUTTON_DEFAULT_DEBOUNCE` which is set to a safe value of 50 ms, but you
can always change that value using the `setDebounceTime` method. Its parameter is a `byte`, since a debounce value should never go above
255.

## Examples
### Single and Double-Click
This example just listens for single and double clicks.
```C++
#include "Arduino.h"
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
```

### Handling All Events
```C++
#include "Arduino.h"
#include "EButton.h"

EButton button(2);

// ------- Printing event details --------
void print(EButton &btn) {
	Serial.print(F(" [pressed="));
	Serial.print(btn.isButtonPressed());
	Serial.print(F(", clicks="));
	Serial.print(btn.getClicks());
	Serial.print(F(", startTime="));
	Serial.print(btn.getStartTime());
	Serial.print(F(", lastTransitionTime="));
	Serial.print(btn.getLastTransitionTime());
	Serial.println(F("]"));
}

// ------- Handler methods --------
void transitionHandler(EButton &btn) {
	Serial.print(F("TRANSITION"));
	print(btn);
}
void eachClickHandler(EButton &btn) {
	Serial.print(F("EACH_CLICK"));
	print(btn);
}
void singleClickHandler(EButton &btn) {
	Serial.print(F("SINGLE_CLICK"));
	print(btn);
}
void doubleClickHandler(EButton &btn) {
	Serial.print(F("DOUBLE_CLICK"));
	print(btn);
}
void anyClickHandler(EButton &btn) {
	Serial.print(F("ANY_CLICK"));
	print(btn);
}

void pressStartHandler(EButton &btn) {
	Serial.print(F("PRESS_START"));
	print(btn);
}

unsigned long t;
void duringPressHandler(EButton &btn) {
	if ((unsigned long) (millis() - t) > 1000) {
		// Print once a second
		Serial.print(F("DURING_PRESS"));
		print(btn);
		t = millis();
	}
}

void pressEndHandler(EButton &btn) {
	Serial.print(F("PRESS_END"));
	print(btn);
}

// ------- Setting up the driver and registering listeners --------
void setup() {
	Serial.begin(115200);
	Serial.println(F("\nEButton Demo"));

	button.setDebounceTime(EBUTTON_DEFAULT_DEBOUNCE);		// not required if using default
	button.setClickTime(EBUTTON_DEFAULT_CLICK);				// not required if using default
	button.setLongPressTime(EBUTTON_DEFAULT_LONG_PRESS);	// not required if using default

	button.attachTransition(transitionHandler);
	button.attachEachClick(eachClickHandler);
	button.attachAnyClick(anyClickHandler);
	button.attachSingleClick(singleClickHandler);
	button.attachDoubleClick(doubleClickHandler);
	button.attachLongPressStart(pressStartHandler);
	button.attachDuringLongPress(duringPressHandler);
	button.attachLongPressEnd(pressEndHandler);
}

void loop() {
	// Ticking the driver in a loop
	button.tick();
}
```

### Brisky Fingers Game
This simple "game" just counts how fast you can click in a sequence.

```C++
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
	button.attachAnyClick(countClicks);
	Serial.println("\nClick as fast as you can!");
}

void loop() {
	button.tick();
}
```
