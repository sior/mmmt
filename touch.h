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
#define MAX_TOUCHES 3
#define NUM_TOUCH_SLOTS 22
#define SWIPE_THRESH 150
#define PINCH_THRESH 500000
#define TAP_TIME 125
#define TAP_TIMEOUT 200

enum touch_flags {
    TOUCH_NEW = 1,
    TOUCH_INITED = 2,
    TOUCH_EVENT = 4,
    TOUCH_ENDED = 8,
    TOUCH_IGNORE_EVENTS = 16,
    TOUCH_FIRST = 32,
    TOUCH_SECOND = 64,
    TOUCH_THIRD = 128,
    TOUCH_FOURTH = 256,
    TOUCH_TWO_TOUCH_EVENT = 512,
    TOUCH_X = 1024,
    TOUCH_Y = 2048,
    TOUCH_TAPPED = 4096
};

typedef struct {
    int x;
    int y;
} point;

typedef struct {
    int id;
    int major;
    int minor;
    int orientation;
    point origin;
    point event;
    point current;
    int flags;
    struct timeval origin_time;
    struct timeval event_time;
    int unknown;
    struct timeval tap_time;
    struct timeval previous_tap_time;
} touch_data;

touch_data touches[NUM_TOUCH_SLOTS];
int num_touches;
int current_touch;
int tapping[NUM_TOUCH_SLOTS];
int touch_one_index;
int touch_two_index;

void touch_activate(int index);

void clear_tapping(void);

void touch_data_clear(int index);

int touch_exists(int id);

int touch_index_for_id(int id);

void touch_new(int id);

void touch_remove(int index);

void touch_add_x(int index, int x);

void touch_add_y(int index, int y);

int touch_dy(int index);

int touch_dx(int index);

int touch_second_index(int index);

int touch_delta(int index1, int index2);

void check_for_tap(int index, struct timeval time);

void tap_timeouts();

void touch_process(int index, struct timeval time);
