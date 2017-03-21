# EButton - Single Event-Based Button Driver

## Why This One?
**`EButton`** is short for "Event Button". I wrote this library in need of a reliable, compact driver for controlling a single key in 
my **`Arduino`** projects, with debouncing being properly handled, and fine grained, well defined events. Besides that, I wanted a
library that I would be able to easily strip of unneeded features, making its memory footprint as small as possible.

## Quick Start
1. Include the library. *(doh!)*
2. Instantiate the **`EButton`** object, specfying a pin (connected to one lead of the button, while its other lead is connected to GND.
3. Write handler method(s).
4. Attaching handlers.
5. Keep executing the object's `tick()` method in a loop. It does all the magic - reads button state changes and triggers appropriate
   events when detected.

```C++
#include "Arduino.h"
// 1. Include the library
#include "EButton.h"

// 2. Instantiate the object and attach it to PIN 2
EButton button(2);

// 3. Handler method for a single-click
void singleClick(EButton &btn) {
	Serial.println("We have a single-click!");
}

void setup() {
	Serial.begin(115200);
// 4. Attach the handler
	button.attachSingleClick(singleClick);
	Serial.println("\nClick or double-click!");
}

void loop() {
// 5. Tick the object in a loop
	button.tick();
}
```

## Clicks and Long-Presses
The class defines two major event types - a **Click** and a **Long-Press**:
* **Click** - an event when the button is released (except when coming from a **Long-Press** state);
* **Long-Press** - when the button is kept pressed at least for a time specified by parameter called `longPressTime`;

> **NOTE:** A *Click* event is not triggered when a button is getting pressed, but rather when it gets released. This is done on
> purpose, in order to be able to tell apart a *Click* from a *Long-Press* event. That is possible only after a long-press 
> timeout, or when the key is released before the timeout. In either case - it can not be at the moment of the key getting pressed.

## Counting Clicks
It is important to define how long the class will be counting clicks, to tell apart if there were two discrete clicks, or one
double-click (or even a 5-click if you're fast enough).

The parameter called `clickTime` is a value in milliseconds, defining how long will the driver wait since the last click, before
wrapping up the count. It does *not* mean that all the clicks have to be performed in that period, but it does specify the maxium
allowed distance between two adjacent clicks.

There are 3 of 8 handler slots available for defining custom methods that act on a given number of clicks:
* `singleClickMethod` - Called if there was a single click;
* `doubleClickMethod` - Called if there was a double click;
* `doneClickingMethod` - Called when clicks counting is done. Use `getClicks()` to get the clicks count.

## Events
Events are listed in their order of appearence:

1. `TRANSITION`- triggered each time the button state changes from pressed to released, or back;
2. `EACH_CLICK` - triggered each time the key is released, unless it was in `LONG_PRESSED` state;
3. `DONE_CLICKING` - triggered after all the clicks have been counted (use `getClicks()` to get the clicks count);
4. `SINGLE_CLICK` - triggered when there was exactly one click;
5. `DOUBLE_CLICK` - triggered when there were exactly two clicks;
6. `LONG_PRESS_START`  triggered once, at the beginning of a long press (after a `TRANSITION` to pressed);
7. `DURING_LONG_PRESS` - triggered on each next tick() while in `LONG_PRESSED` state (*not in the same cycle with `LONG_PRESS_START`*);
8. `LONG_PRESS_END` - triggered once, at the end of a long press (after a `TRANSITION` to released).

Generally, in most of cases it will be enough to handle a SINGLE_CLICK, but that is up to you.

> **NOTE:** It is important to stress the difference between `EACH_CLICK` and `DONE_CLICKING`: the first one is called **each time**
> the key is released (unless it was a long-press), while `DONE_CLICKING` is called **once**, at the end of clicks counting.

## Mixing Clicks and Presses
Events `DONE_CLICKING`, `SINGLE_CLICK`, and `DOUBLE_CLICK` will be triggered *only after the last click*.

Therefore, if you perform a click, immediatelly followed by a long press, the `DONE_CLICKING` and `SINGLE_CLICK` events *will not be
triggered*, and that is by design! Instead, the following sequence of events will be triggered:

1. `TRANSITION` (on the first key-down)
2. `TRANSITION` (on fthe irst key-release)
3. `EACH_CLICK` (after the TRANSITION of the first-key release)
4. `TRANSITION` (on the second key-down)
5. `LONG_PRESS_START` (1 second after the key was pressed and held pressed)
6. `DURING_LONG_PRESS` (on each next tick until the key is released)
7. `LONG_PRESS_END` (when the key is finally released)

Furhermore, *after the long-Press has ended, the button's state will be reset*. This will result in any further clicks that might be
following the long press, being interpreted as a new series of clicks, separate from the previous sequence.

## Handler Methods
For each event there is a slot where you can slip your own methods that is triggered when a corresponding event is detected.

A handler method can be any method returning `void` and accepting one `EButton&` parameter. 
```C++
void doneClicking(EButton &btn) {
   Serial.print("Counted clicks: ");
   Serial.println(btn.getClicks());
}
```
All handler methods are optional, and initially set to `NULL`.

> **NOTE:** Trigger methods should be **short and fast**. Instead of running a complex operation in a handler method, rather use it
> to set a flag indicating that a specific operation has to be performed later on in the code. This especially applies to attempting
> recursive calls to the `tick()` method from within a handler method - which should be avoided, because it will most likely end in 
> tears! 

## Minimizing Memory Footprint
If the memory becomes an issue in your project, you can easily decrease the driver's footprint by disabling support for unneeded events.
That way you can make significant savings in your memory-critical projects.

To disable a feature, just comment out its corresponding `#define` entry in the driver's header file:
```C++
#define EBUTTON_SUPPORT_TRANSITION
#define EBUTTON_SUPPORT_EACH_CLICK
#define EBUTTON_SUPPORT_DONE_CLICKING
#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS
#define EBUTTON_SUPPORT_LONG_PRESS_START
#define EBUTTON_SUPPORT_LONG_PRESS_DURING
#define EBUTTON_SUPPORT_LONG_PRESS_ENG
```
> **NOTE:** If you disable `EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS`, then you can use the `DONE_CLICKING` event to process
> single, double, and any other number of clicks. Just use `getClicks()` to get the final clicks count. You can then also disable
> all other features that you don't need.
>
> Another way of getting a small footprint in simple cases where you just need to **detect each click, regardless their count**, is to
> disable all features except the `EBUTTON_SUPPORT_EACH_CLICK`.

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
This example will give you a good picture about the order in which the events are processed.
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
	Serial.print(F(", prevTransitionTime="));
	Serial.print(btn.getPrevTransitionTime());
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
void doneClickingHandler(EButton &btn) {
	Serial.print(F("DONE_CLICKING"));
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
	button.attachDoneClicking(doneClickingHandler);
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
	button.attachDoneClicking(countClicks);
	Serial.println("\nClick as fast as you can!");
}

void loop() {
	button.tick();
}
```
--------------------------------------------------
## Library Reference
### `EButtonEventHandler` type

- `typedef void (*EButtonEventHandler)(EButton&);` - references to handler methods.

### `EButton` class

- `EButton(byte pin, bool pressedLow = true)` - Constructor specifying attached pin and used logic. `pressedLow` is `true`,
  if using a pull-down switch.
  
- `void` `setDebounceTime(byte time)` - Setting debounce time in milliseconds. Default is `EBUTTON_DEFAULT_DEBOUNCE`.

- `void` `setClickTime(byte time)` - Setting delay after the button was released, when clicks counting ends. In other words,
   this is a delay before triggering `singleClick`, `doubleClick`, or `doneClicking event`. Default is `EBUTTON_DEFAULT_CLICK`.
  
- `void` `setLongPressTime(byte time)` - Setting minimum time that the button has to be pressed in order to start LONG_PRESSED state.
   Default is `EBUTTON_DEFAULT_LONG_PRESS`.

- `void` `attachTransition(EButtonEventHandler methods)` - Attaches a method that is triggered on each transition (state change).

- `void` `attachEachClick(EButtonEventHandler methods)` - Attaches a method that is triggered each time the key goes up (gets
   released), while not in LONG_PRESSED state.

- `void` `attachDoneClicking(EButtonEventHandler methods)` - Attaches a method that is triggered after all the clicks have been counted.
   
- `void` `attachSingleClick(EButtonEventHandler methods)` - Attaches a method that is triggered when there was exactly one click.

- `void` `attachDoubleClick(EButtonEventHandler methods)` - Attaches a method that is triggered when there were exactly two clicks.

- `void` `attachLongPressStart(EButtonEventHandler methods)` - Attaches a method that is triggered once, at the beginning of a long
   press.
   
- `void` `attachDuringLongPress(EButtonEventHandler methods)` - Attaches a method that is triggered on each `tick()` during a long
   press.

- `void` `attachLongPressEnd(EButtonEventHandler methods)` - Attaches a method that is triggered once, at the end of a long press.

- `void` `reset()` - Reset state.

- `void` `tick()` - Update/tick the button. This method has to be called in a loop.

- `byte` `getPin()` - Returns attached pin number.

- `byte` `getClicks()` - Returns number of performed clicks.

- `bool` `isButtonPressed()` - Test if the button was pressed the last time it was sampled.

- `bool` `isLongPressed()` - Test it the button is in long-pressed state.

- `unsigned long` `getStartTime()` - Returns the time of the first button press.

- `unsigned long` `getPrevTransitionTime()` - Time of a previous transition. Returns startTime for the first transition.

## Operators
- `bool` `operator==(EButton &other)` - Tests if the two have the same address.

--------------------------------------------------

## Version History

* `1.0.0 (2017-02-18)`: Original release
* `1.1.0 (2017-02-23)`: Discrete enabling/disabling START, DURING and END support for LONG_PRESS
