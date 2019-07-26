/*
 * EButton v1.2.0 - Customizable button-driver class with a small footprint, supporting debouncing, and various events.
 *
 * Its already small footprint can be additionally minimized by disabling unneeded features using #define switches.
 *
 * Definitions
 * -----------
 * - "Click" - an event when the button is released, and it was not in a LONG_PRESSED state;
 * - "Long-press" - when the button is kept pressed at least for a time specified by longPressTime;
 * - "Debounce" - time after pressing or releasing a key, while the state readings oscillate between LOW and HIGH, due to
 *   imperfection of electrical contact;
 *
 *
 * Supported Events:
 * -----------------
 * - TRANSITION			- triggered each time the button state changes from pressed to released, or back
 * - EACH_CLICK			- triggered each time the key is released, unless it was in LONG_PRESSED state
 * - DONE_CLICKING		- triggered after all the clicks have been counted (use getClicks() to get the clicks count)
 * - SINGLE_CLICK		- triggered when there was exactly one click
 * - DOUBLE_CLICK		- triggered when there were exactly two clicks
 * - LONG_PRESS_START	- triggered once, at the beginning of a long press (after TRANSITION to pressed)
 * - DURING_LONG_PRESS	- triggered on each tick() while in LONG_PRESSED state
 * - LONG_PRESS_END		- triggered once, at the end of a long press (after TRANSITION to released)
 *
 *
 * Handler methods
 * ---------------
 * You can assign a different method for each of the above events. These methods accept the Button& as the parameter,
 * allowing you to read details about the event, like a number of clicks, time of the first click, etc.
 *
 *
 *     Version: 1.2.0
 *     License: MIT
 *  Created on: 2017-02-27
 *      Author: JonnieZG
 */

#ifndef EBUTTON_H_
#define EBUTTON_H_

#include "Arduino.h"

// --------------------------- Optional Feature Switches ---------------------------
// You can disable the features you don't need, to minimize memory footprint
// Just include some of the following defines before including EButton.h and
// don't forget to clean the project after making changes to these options!

//#define EBUTTON_SUPPORT_TRANSITION_DISABLED
//#define EBUTTON_SUPPORT_EACH_CLICK_DISABLED
//#define EBUTTON_SUPPORT_DONE_CLICKING_DISABLED
//#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS_DISABLED
//#define EBUTTON_SUPPORT_LONG_PRESS_START_DISABLED
//#define EBUTTON_SUPPORT_LONG_PRESS_DURING_DISABLED


#ifndef EBUTTON_SUPPORT_TRANSITION_DISABLED
#define EBUTTON_SUPPORT_TRANSITION
#endif
#ifndef EBUTTON_SUPPORT_EACH_CLICK_DISABLED
#define EBUTTON_SUPPORT_EACH_CLICK
#endif
#ifndef EBUTTON_SUPPORT_DONE_CLICKING_DISABLED
#define EBUTTON_SUPPORT_DONE_CLICKING
#endif
#ifndef EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS_DISABLED
#define EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS
#endif
#ifndef EBUTTON_SUPPORT_LONG_PRESS_START_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_START
#endif
#ifndef EBUTTON_SUPPORT_LONG_PRESS_DURING_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_DURING
#endif
#ifndef EBUTTON_SUPPORT_LONG_PRESS_END_DISABLED
#define EBUTTON_SUPPORT_LONG_PRESS_END
#endif

// -------- Default Timings in milliseconds (can be modified using setters) --------
#define EBUTTON_DEFAULT_DEBOUNCE		50
#if defined(EBUTTON_SUPPORT_DONE_CLICKING) || defined(EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS)
#define EBUTTON_DEFAULT_CLICK			150
#endif
#if defined(EBUTTON_SUPPORT_LONG_PRESS_START) || defined(EBUTTON_SUPPORT_LONG_PRESS_DURING) || defined(EBUTTON_SUPPORT_LONG_PRESS_END)
#define EBUTTON_DEFAULT_LONG_PRESS		1000
#endif
// ---------------------------------------------------------------------------------
// <<<< END OF CONFIGURABLE OPTIONS

#define EBUTTON_STATE_IDLE					0
#define EBUTTON_STATE_COUNTING_CLICKS_DOWN	1
#define EBUTTON_STATE_COUNTING_CLICKS_UP	2
#if defined(EBUTTON_SUPPORT_LONG_PRESS_START) || defined(EBUTTON_SUPPORT_LONG_PRESS_DURING) || defined(EBUTTON_SUPPORT_LONG_PRESS_END)
#define EBUTTON_STATE_LONG_PRESSED			3
#endif
class EButton;
extern "C" {
// Pointer to event handling methods
typedef void (*EButtonEventHandler)(EButton&);
}

class EButton {
public:
	// Constructor.
	EButton(byte pin, bool pressedLow = true);

	// Debounce time - delay after the first transition, before sampling the next state.
	void setDebounceTime(byte time);
	// Click time - delay after the button was released, when clicks counting ends.
	// (Delay before triggering singleClick, doubleClick, or doneClicking event.)
#if defined(EBUTTON_SUPPORT_DONE_CLICKING) || defined(EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS)
	void setClickTime(unsigned int time);
#endif
#if defined(EBUTTON_SUPPORT_LONG_PRESS_START) || defined(EBUTTON_SUPPORT_LONG_PRESS_DURING) || defined(EBUTTON_SUPPORT_LONG_PRESS_END)
	// Long-Press time - minimum time to keep the button pressed in order to start LONG_PRESSED state.
	void setLongPressTime(unsigned int time);
#endif
#ifdef EBUTTON_SUPPORT_TRANSITION
	// Attaches a method that is triggered on each transition (state change) - triggered first.
	void attachTransition(EButtonEventHandler method);
#endif
#ifdef EBUTTON_SUPPORT_EACH_CLICK
	// Attaches a method that is triggered each time the key goes up, while not in LONG_PRESSED state - triggered second
	void attachEachClick(EButtonEventHandler method);
#endif
#ifdef EBUTTON_SUPPORT_DONE_CLICKING
	// Attaches a method that is triggered after all the clicks have been counted - triggered third
	void attachDoneClicking(EButtonEventHandler method);
#endif
#ifdef EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS
	// Attaches a method that is triggered when there was exactly one click - triggered fourth
	void attachSingleClick(EButtonEventHandler method);
	// Attaches a method that is triggered when there were exactly two clicks - triggered fifth
	void attachDoubleClick(EButtonEventHandler method);
#endif
#ifdef EBUTTON_SUPPORT_LONG_PRESS_START
	// Attaches a method that is triggered once, at the beginning of a long press - triggered after the transition to pressed
	void attachLongPressStart(EButtonEventHandler method);
#endif
#ifdef EBUTTON_SUPPORT_LONG_PRESS_DURING
	// Attaches a method that is triggered on each tick() during a long press - triggered after pressStart
	void attachDuringLongPress(EButtonEventHandler method);
#endif
#ifdef EBUTTON_SUPPORT_LONG_PRESS_END
	// Attaches a method that is triggered once, at the end of a long press - triggered after transition to released
	void attachLongPressEnd(EButtonEventHandler method);
#endif
	// Reset state
	void reset();

	// Update/tick the button
	void tick();

	// Attached pin number
	byte getPin();

	// Number of clicks performed
	byte getClicks();

	// Test if the button was pressed the last time it was sampled
	bool isButtonPressed();

#if defined(EBUTTON_SUPPORT_LONG_PRESS_START) || defined(EBUTTON_SUPPORT_LONG_PRESS_DURING) || defined(EBUTTON_SUPPORT_LONG_PRESS_END)
	// Test it the button is in long-pressed state
	bool isLongPressed();
#endif

	// The time of the first button press
	unsigned long getStartTime();

	// Time of the previous transition
	unsigned long getPrevTransitionTime();

	// Tests if the two have the same address
	bool operator==(EButton &other);

private:
	void transition(unsigned long now);

	// ----- Configuration-specific fields -----
	byte pin;										// Attached pin
	byte debounceTime = EBUTTON_DEFAULT_DEBOUNCE;	// Debounce time in ms (between 0 and 255)
#if defined(EBUTTON_SUPPORT_DONE_CLICKING) || defined(EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS)
	unsigned int clickTime = EBUTTON_DEFAULT_CLICK;	// Time the button has to be released in order to complete counting clicks
#endif
#if defined(EBUTTON_SUPPORT_LONG_PRESS_START) || defined(EBUTTON_SUPPORT_LONG_PRESS_DURING) || defined(EBUTTON_SUPPORT_LONG_PRESS_END)
	unsigned int longPressTime = EBUTTON_DEFAULT_LONG_PRESS;	// Minimum press time
#endif
	bool pressedState;								// Logical value of the pressed state

#ifdef EBUTTON_SUPPORT_TRANSITION
	EButtonEventHandler transitionMethod = NULL;
#endif
#ifdef EBUTTON_SUPPORT_EACH_CLICK
	EButtonEventHandler eachClickMethod = NULL;
#endif
#ifdef EBUTTON_SUPPORT_SINGLE_AND_DOUBLE_CLICKS
	EButtonEventHandler singleClickMethod = NULL;
	EButtonEventHandler doubleClickMethod = NULL;
#endif
#ifdef EBUTTON_SUPPORT_DONE_CLICKING
	EButtonEventHandler doneClickingMethod = NULL;
#endif
#ifdef EBUTTON_SUPPORT_LONG_PRESS_START
	EButtonEventHandler longPressStartMethod = NULL;
#endif
#ifdef EBUTTON_SUPPORT_LONG_PRESS_DURING
	EButtonEventHandler duringLongPresstMethod = NULL;
#endif
#ifdef EBUTTON_SUPPORT_LONG_PRESS_END
	EButtonEventHandler longPressEndMethod = NULL;
#endif
	// ----- State-specific fields -----
	byte state;								// Current FSM state
	bool buttonPressed;						// last button state
	unsigned long startTime;				// when the first click was detected
	unsigned long prevTransitionTime = 0;	// previous time the button state has changed UP->DOWN, or DOWN->UP
	byte clicks;							// Number of clicks performed
};

#endif /* EBUTTON_H_ */
