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

#ifndef EVENTSH
#define EVENTSH

#define SHIFT_FLAG 1
#define ALT_FLAG 2
#define CONTROL_FLAG 4
#define SUPER_FLAG 8
#define SINGLE_FLAG 16

#define EVENT_SWIPE_UP_ONE      0
#define EVENT_SWIPE_DOWN_ONE    1
#define EVENT_SWIPE_LEFT_ONE    2
#define EVENT_SWIPE_RIGHT_ONE   3
#define EVENT_SWIPE_UP_TWO      4
#define EVENT_SWIPE_DOWN_TWO    5
#define EVENT_SWIPE_LEFT_TWO    6
#define EVENT_SWIPE_RIGHT_TWO   7
#define NUM_EVENT_TYPES         8 

#define EVENT_NONE 0
#define EVENT_MOUSE 1
#define EVENT_KEY 2

void initEvents();
int runEvent(int type);
#endif
