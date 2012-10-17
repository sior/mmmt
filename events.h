/*
Copyright (C) 2012 Matthew Knopf

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
#define SHIFT_FLAG 1
#define ALT_FLAG 2
#define CONTROL_FLAG 4
#define SUPER_FLAG 8
#define SINGLE_FLAG 16
#define KINETIC_FLAG 32
#define NUM_EVENTS 12

enum touch_event_type {
    SWIPE_DOWN_ONE_FINGER,
    SWIPE_UP_ONE_FINGER,
    SWIPE_LEFT_ONE_FINGER,
    SWIPE_RIGHT_ONE_FINGER,
    SWIPE_DOWN_TWO_FINGERS,
    SWIPE_UP_TWO_FINGERS,
    SWIPE_LEFT_TWO_FINGERS,
    SWIPE_RIGHT_TWO_FINGERS,
    PINCH_IN,
    PINCH_OUT,
    DOUBLE_TAP_ONE_FINGER,
    DOUBLE_TAP_TWO_FINGERS
};

typedef struct {
    int mouse;
    int key;
    int flags;
} touch_event;

touch_event event_settings[NUM_EVENTS];

void load_settings();

void trigger_event(int type, int index, struct timeval time);
