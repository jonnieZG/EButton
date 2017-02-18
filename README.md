# EButton

## Why This One?
I wrote this library in need of a reliable, compact driver for controlling a single key in my **`Arduino`** projects, with debouncing
being properly handled, and fine grained, well defined events. Besides that, I wanted a library that I would be able to easily strip of
unneeded features, making its memory footprint as small as possible.

## Quick Start
1. Include the library. (doh!)
2. Instantiate the **`EButton`** object, specfying a pin (the pin is connected to one lead of the button, while its other lead is
   connected to GND.
3. Write handler method(s) and attach them to the class.
4. Keep executing the object's `tick()` method in a loop. It does all the magic - reads button state changes and triggers appropriate
   events when detected.

## Clicks and Long-Presses
The class defines two major event types - a **Click** and a **Long-Press**:
* **Click** - an event when the button is released, and it was not in a LONG_PRESSED state;
* **Long-Press** - when the button is kept pressed at least for a time specified by parameter called `longPressTime`;

## Counting Clicks
It is important to define how long the class will be counting clicks, to tell apart if a button was clicked twice, and if there
was a double-click (or even a 5-click if you're fast enough).

The parameter called `clickTime` is a value in milliseconds, defining how long will the driver wait since the last click, before
wrapping up the count. It does *not* mean that all the clicks have to be performed in that period, but it does specify the maxium
allowed distance between two adjacent clicks that form a group.

## Events
Events are listed in their order of appearence:

1. `TRANSITION`- triggered each time the button state changes from pressed to released, or back;
2. `EACH_CLICK` - triggered each time the key is released, unless it was in `LONG_PRESSED` state;
3. `ANY_CLICK` - triggered after all the clicks have been counted (use getClicks() to get the clicks count);
4. `SINGLE_CLICK` - triggered when there was exactly one click;
5. `DOUBLE_CLICK` - triggered when there were exactly two clicks;
6. `LONG_PRESS_START`  triggered once, at the beginning of a long press (after a `TRANSITION` to pressed);
7. `DURING_LONG_PRESS` - triggered on each tick() while in `LONG_PRESSED` state;
8. `LONG_PRESS_END` - triggered once, at the end of a long press (after a `TRANSITION` to released).

Generally, in most of cases it will be enough to handle a SINGLE_CLICK, but that is up to you.

**NOTE:** It is important to stress the difference between `EACH_CLICK` and `ANY_CLICK`: the first one is called **each time**
the key is released (unless it was a long-press), while `ANY_CLICK` is called **once**, at the end of clicks counting.


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

