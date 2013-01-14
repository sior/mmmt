/*
Copyright (C) 2013 Matthew Knopf

Permission is hereby granted, free of charge, 
to any person obtaining a copy of this software 
and associated documentation files (the "Software"), 
to deal in the Software without restriction, 
including without limitation the rights to use, 
copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission 
notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include "events.h"

#include <ncurses.h>
Display *display;
int eventKind[NUM_EVENT_TYPES];
int eventCodes[NUM_EVENT_TYPES];
int eventFlags[NUM_EVENT_TYPES];

void initEvents()
{
    display = XOpenDisplay(NULL);
    
    eventKind[EVENT_SWIPE_UP_ONE] = EVENT_MOUSE;
    eventCodes[EVENT_SWIPE_UP_ONE] = 4;
    eventFlags[EVENT_SWIPE_UP_ONE] = 0;

    eventKind[EVENT_SWIPE_DOWN_ONE] = EVENT_MOUSE;
    eventCodes[EVENT_SWIPE_DOWN_ONE] = 5;
    eventFlags[EVENT_SWIPE_DOWN_ONE] = 0;

    eventKind[EVENT_SWIPE_LEFT_ONE] = EVENT_MOUSE;
    eventCodes[EVENT_SWIPE_LEFT_ONE] = 6;
    eventFlags[EVENT_SWIPE_LEFT_ONE] = 0;

    eventKind[EVENT_SWIPE_RIGHT_ONE] = EVENT_MOUSE;
    eventCodes[EVENT_SWIPE_RIGHT_ONE] = 7;
    eventFlags[EVENT_SWIPE_RIGHT_ONE] = 0;

    eventKind[EVENT_SWIPE_UP_TWO] = EVENT_KEY;
    eventCodes[EVENT_SWIPE_UP_TWO] = 112;
    eventFlags[EVENT_SWIPE_UP_TWO] = 0 | SINGLE_FLAG;

    eventKind[EVENT_SWIPE_DOWN_TWO] = EVENT_KEY;
    eventCodes[EVENT_SWIPE_DOWN_TWO] = 117;
    eventFlags[EVENT_SWIPE_DOWN_TWO] = 0 | SINGLE_FLAG;

    eventKind[EVENT_SWIPE_LEFT_TWO] = EVENT_KEY;
    eventCodes[EVENT_SWIPE_LEFT_TWO] = 113;
    eventFlags[EVENT_SWIPE_LEFT_TWO] = 0 | ALT_FLAG | SINGLE_FLAG;

    eventKind[EVENT_SWIPE_RIGHT_TWO] = EVENT_KEY;
    eventCodes[EVENT_SWIPE_RIGHT_TWO] = 114;
    eventFlags[EVENT_SWIPE_RIGHT_TWO] = 0 | ALT_FLAG | SINGLE_FLAG;
}

int runEvent(int type)
{
    if (eventKind[type] == EVENT_NONE)
        return 0;

    if (eventFlags[type] & ALT_FLAG)
        XTestFakeKeyEvent(display, 64, True, CurrentTime);
    if (eventFlags[type] & SHIFT_FLAG)
        XTestFakeKeyEvent(display, 50, True, CurrentTime);
    if (eventFlags[type] & CONTROL_FLAG)
        XTestFakeKeyEvent(display, 37, True, CurrentTime);
    if (eventFlags[type] & SUPER_FLAG)
        XTestFakeKeyEvent(display, 133, True, CurrentTime);
    
    if (eventKind[type] == EVENT_KEY){
        XTestFakeKeyEvent(display, eventCodes[type], True, CurrentTime);
        XTestFakeKeyEvent(display, eventCodes[type], False, CurrentTime);
    }
    if (eventKind[type] == EVENT_MOUSE){
        XTestFakeButtonEvent(display, eventCodes[type], True, CurrentTime);
        XTestFakeButtonEvent(display, eventCodes[type], False, CurrentTime);
    }
    
    if (eventFlags[type] & ALT_FLAG)
        XTestFakeKeyEvent(display, 64, False, CurrentTime);
    if (eventFlags[type] & SHIFT_FLAG)
        XTestFakeKeyEvent(display, 50, False, CurrentTime);
    if (eventFlags[type] & CONTROL_FLAG)
        XTestFakeKeyEvent(display, 37, False, CurrentTime);
    if (eventFlags[type] & SUPER_FLAG)
        XTestFakeKeyEvent(display, 133, False, CurrentTime);
    
    XFlush(display);
    return (eventFlags[type] & SINGLE_FLAG);
}
