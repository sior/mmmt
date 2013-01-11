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
#include <stdio.h>
#include <string.h>
#include <X11/Xutil.h>
#include "touch.h"
#include "util.h"
#include "events.h"

void touch_activate(int index){
    touches[index].flags |= TOUCH_INITED;
    num_touches++;
}

void clear_tapping(void){
    memset(tapping, 0, sizeof(tapping));
}

void touch_data_clear(int index){
    //if there was a tap, preserve tap data
    struct timeval tmp;
    if (touches[index].flags & TOUCH_TAPPED){
        tmp = touches[index].tap_time;
        memset(&touches[index], 0, sizeof(touch_data));
        touches[index].id = -1;
        touches[index].previous_tap_time = tmp;
        touches[index].flags |= TOUCH_TAPPED;
    } else {
        memset(&touches[index], 0, sizeof(touch_data));
        touches[index].id = -1;
    }
}

int touch_exists(int id){
    int i;
    for (i = 0; i < NUM_TOUCH_SLOTS; i++){
        if (touches[i].id == id){ return 1;}
    }
    return 0;
}

int touch_index_for_id(int id){
    int i;
    for (i = 0; i < NUM_TOUCH_SLOTS; i++){
        if (touches[i].id == id){ return i;}
    }
    return -1;
}

void touch_new(int id){
    if (num_touches < MAX_TOUCHES){
        //touches[id].flags = 0;
        touches[id].flags |= TOUCH_NEW;
        switch(num_touches){
            case 0:
                touches[id].flags |= TOUCH_FIRST;
                touch_one_index = id;
                touch_two_index = -1;
                break;
            case 1:
                touches[id].flags |= TOUCH_SECOND;
                touch_two_index = id;
                break;
            case 2:
                touches[id].flags |= TOUCH_THIRD;
                break;
            case 3:
                touches[id].flags |= TOUCH_FOURTH;
                break;
        }
        num_touches++;
    }
}

void touch_remove(int index){
    if (num_touches > 0){
        if ((num_touches == 2) && (touches[index].flags & TOUCH_TWO_TOUCH_EVENT)){
            if (touch_one_index == index){
                touch_one_index = touch_two_index;
                touch_two_index = -1;
                touches[touch_one_index].flags |= TOUCH_IGNORE_EVENTS;
            } else {
                touches[touch_one_index].flags |= TOUCH_IGNORE_EVENTS;
                touch_two_index = -1;
            }
        } else if (num_touches == 1) {
            touch_one_index = -1;
        }
        //touches[index].flags = 0;
        touch_data_clear(index);
        num_touches--;
    }
}

void touch_add_x(int index, int x){
    if (!(touches[index].flags & TOUCH_X)){
        touches[index].origin.x = x;
        touches[index].flags |= TOUCH_X;
    }
    touches[index].current.x = x;
};

void touch_add_y(int index, int y){
    if (!(touches[index].flags & TOUCH_Y)){
        touches[index].origin.y = y;
        touches[index].flags |= TOUCH_Y;
    }
    touches[index].current.y = y;
};


int touch_dy(int index){
    if (touches[index].flags & TOUCH_EVENT){
        return touches[index].current.y - touches[index].event.y;
    } else {
        return touches[index].current.y - touches[index].origin.y;
    }
};

int touch_dx(int index){
    if (touches[index].flags & TOUCH_EVENT){
        return touches[index].current.x - touches[index].event.x;
    } else {
        return touches[index].current.x - touches[index].origin.x;
    }
};

int touch_second_index(int index){
    if (index == touch_one_index)
        return touch_two_index;
    else
        return touch_one_index;
};

int touch_delta(int index1, int index2){
    point one, two;
    if (touches[index1].flags & TOUCH_EVENT){
        one = touches[index1].event;
    } else {
        one = touches[index1].origin;
    }
    if (touches[index2].flags & TOUCH_EVENT){
        two = touches[index2].event;
    } else {
        two = touches[index2].origin;
    }
    int dx1 = one.x - two.x;
    int dy1 = one.y - two.y;
    int dx2 = touches[index1].current.x - touches[index2].current.x;
    int dy2 = touches[index1].current.y - touches[index2].current.y;
    int d1 = (dx1 * dx1) + (dy1 * dy1);
    int d2 = (dx2 * dx2) + (dy2 * dy2);
    return d2 - d1;
};

void check_for_tap(int index, struct timeval time)
{
    if (timevaldiff(touches[index].origin_time, time) < TAP_TIME){
        struct timeval tmp;
        gettimeofday(&tmp, NULL);
        touches[index].tap_time = time;
        touches[index].flags |= TOUCH_TAPPED;
        tapping[index]++;
    }
}

void tap_timeouts()
{
    int i;
    struct timeval time;
    gettimeofday(&time, NULL);
    for (i = 0; i < NUM_TOUCH_SLOTS; i++){
        if (tapping[i]){
            if (touches[i].flags & TOUCH_TAPPED){
                if (timevaldiff(touches[i].previous_tap_time, time) > TAP_TIMEOUT){
                    touches[i].flags &= ~TOUCH_TAPPED;
                    if (tapping[i] == 2){
                        trigger_event(DOUBLE_TAP_ONE_FINGER, i, time);
                    }
                    tapping[i] = 0;
                }
            }
        }
    }
}

void touch_process(int index, struct timeval time){
    if (touches[index].flags == 0){
        return;
    }
    
    if (touches[index].flags & TOUCH_NEW){
        touches[index].origin_time = time;
        touches[index].flags &= ~(TOUCH_NEW);
        touches[index].flags |= TOUCH_INITED;
    }
    
    if (touches[index].flags & TOUCH_IGNORE_EVENTS){
        return;
    }
    if (num_touches == 1){
        //printf("process one touch\n");
        int dx = touch_dx(index);
        int dy = touch_dy(index);

        if (abs(dy) > SWIPE_THRESH){ 
            if (dy < 0){
                int i;
                trigger_event(SWIPE_UP_ONE_FINGER, index, time); 
            } else {
                int i;
                trigger_event(SWIPE_DOWN_ONE_FINGER, index, time);
            }
        }
        if (abs(dx) > SWIPE_THRESH){
            if (dx < 0){
                int i;
                trigger_event(SWIPE_LEFT_ONE_FINGER, index, time);
            } else {    
                int i;
                trigger_event(SWIPE_RIGHT_ONE_FINGER, index, time);
            }
        }
    }

    if (num_touches == 2){
// Tried to implement pinching, but this doesn't seem very reliable
/*        if (touches[index].flags & TOUCH_FIRST){
            int delta = touch_delta(index, touch_second_index(index));
            if (abs(delta) > PINCH_THRESH){
                if (delta > 0){
                    trigger_event(PINCH_OUT, index, time);
                    return;
                } else {
                    trigger_event(PINCH_IN, index, time);
                    return;
                }
            }
        }
*/
        int dy = touch_dy(index);
        int dx = touch_dx(index);
        int dy2 = touch_dy(touch_second_index(index));
        int dx2 = touch_dx(touch_second_index(index));
        if ((abs(dy) > SWIPE_THRESH) && (abs(dy2) > SWIPE_THRESH)){
            if ((dy < 0) && (dy2 < 0)){
                trigger_event(SWIPE_UP_TWO_FINGERS, index, time);
            } else if ((dy > 0) && (dy2 > 0)){
                trigger_event(SWIPE_DOWN_TWO_FINGERS, index, time);
            }
        }
        if ((abs(dx) > SWIPE_THRESH) && (abs(dx2) > SWIPE_THRESH)){
            if ((dx < 0) && (dx2 < 0)){
                trigger_event(SWIPE_LEFT_TWO_FINGERS, index, time);
            } else if ((dx > 0) && (dx2 > 0)){
                trigger_event(SWIPE_RIGHT_TWO_FINGERS, index, time);
            }
        }
    }
}
