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

int tenpow(int pow){
    int i, result = 1;
    for (i = 0; i < pow; i++)
        result*=10;
    return result;
}

void load_settings(){
    char buf[100];
    FILE *fp = fopen("mmmtrc", "r");
    int current_event = -1;
    memset(event_settings, 0, sizeof(touch_event) * NUM_EVENTS);
    if (fp){
        while (!feof(fp)){
            fgets(buf, 99, fp);
            if (buf[0] == '#')
                continue;
            if (strcmp(buf, "Swipe Down One Finger\n") == 0)
                current_event = SWIPE_DOWN_ONE_FINGER; 
            if (strcmp(buf, "Swipe Up One Finger\n") == 0)
                current_event = SWIPE_UP_ONE_FINGER;
            if (strcmp(buf, "Swipe Left One Finger\n") == 0)
                current_event = SWIPE_LEFT_ONE_FINGER;
            if (strcmp(buf, "Swipe Right One Finger\n") == 0)
                current_event = SWIPE_RIGHT_ONE_FINGER;
            if (strcmp(buf, "Swipe Down Two Fingers\n") == 0)
                current_event = SWIPE_DOWN_TWO_FINGERS;
            if (strcmp(buf, "Swipe Up Two Fingers\n") == 0)
                current_event = SWIPE_UP_TWO_FINGERS;
            if (strcmp(buf, "Swipe Left Two Fingers\n") == 0)
                current_event = SWIPE_LEFT_TWO_FINGERS;
            if (strcmp(buf, "Swipe Right Two Fingers\n") == 0)
                current_event = SWIPE_RIGHT_TWO_FINGERS;
            if (strcmp(buf, "Pinch In\n") == 0)
                current_event = PINCH_IN;
            if (strcmp(buf, "Pinch Out\n") == 0)
                current_event = PINCH_OUT;
            if (strcmp(buf, "Double Tap One Finger\n") == 0)
                current_event = DOUBLE_TAP_ONE_FINGER;
            if (strcmp(buf, "Doube Tap Two Fingers\n") == 0)
                current_event = DOUBLE_TAP_TWO_FINGERS;
            if (strcmp(buf, "Shift\n") == 0)
                if (current_event != -1)
                    event_settings[current_event].flags |= SHIFT_FLAG;
            if (strcmp(buf, "Alt\n") == 0)
                if (current_event != -1)
                    event_settings[current_event].flags |= ALT_FLAG;
            if (strcmp(buf, "Control\n") == 0)
                if (current_event != -1)
                    event_settings[current_event].flags |= CONTROL_FLAG;
            if (strcmp(buf, "Super\n") == 0)
                if (current_event != -1)
                    event_settings[current_event].flags |= SUPER_FLAG;
            if (strcmp(buf, "Single Action\n") == 0)
                if (current_event != -1)
                    event_settings[current_event].flags |= SINGLE_FLAG;
            if (strstr(buf, "Mouse Button:"))
                if (current_event != -1){
                    int l = strlen(buf);
                    int i;
                    int code = 0;
                    for (i = 0; i < l - 14; i++){
                        code += tenpow(i) * (buf[l - i - 2] - 48);
                    }
                    event_settings[current_event].mouse = code;
                }
            if (strstr(buf, "Key Code:"))
                if (current_event != -1){
                    int l = strlen(buf);
                    int i;
                    int code = 0;
                    for (i = 0; i < l - 10; i++){
                        code += tenpow(i) * (buf[l - i - 2] - 48);
                    }
                    event_settings[current_event].key = code;
                }

        }
    }
    fclose(fp);
}

void trigger_event(int type, int index, struct timeval time){
    if (event_settings[type].flags & ALT_FLAG)
        XTestFakeKeyEvent(dpy, 64, True, CurrentTime);
    if (event_settings[type].flags & SHIFT_FLAG)
        XTestFakeKeyEvent(dpy, 50, True, CurrentTime);
    if (event_settings[type].flags & CONTROL_FLAG)
        XTestFakeKeyEvent(dpy, 37, True, CurrentTime);
    if (event_settings[type].flags & SUPER_FLAG)
        XTestFakeKeyEvent(dpy, 133, True, CurrentTime);
    
    if (event_settings[type].mouse){
        XTestFakeButtonEvent(dpy, event_settings[type].mouse, True, CurrentTime);
        XTestFakeButtonEvent(dpy, event_settings[type].mouse, False, CurrentTime);
    }
    
    if (event_settings[type].key){
        XTestFakeKeyEvent(dpy, event_settings[type].key, True, CurrentTime);
        XTestFakeKeyEvent(dpy, event_settings[type].key, False, CurrentTime);
    }
    
    if (event_settings[type].flags & ALT_FLAG)
        XTestFakeKeyEvent(dpy, 64, False, CurrentTime);
    if (event_settings[type].flags & SHIFT_FLAG)
        XTestFakeKeyEvent(dpy, 50, False, CurrentTime);
    if (event_settings[type].flags & CONTROL_FLAG)
        XTestFakeKeyEvent(dpy, 37, False, CurrentTime);
    if (event_settings[type].flags & SUPER_FLAG)
        XTestFakeKeyEvent(dpy, 133, False, CurrentTime);
    
    XFlush(dpy);

    touches[index].event = touches[index].current;
    touches[index].flags |= TOUCH_EVENT;
    touches[index].event_time = time;
    if (event_settings[type].flags & SINGLE_FLAG)
        touches[index].flags |= TOUCH_IGNORE_EVENTS;
        
    if (num_touches == 2){
        int second = touch_second_index(index);
        touches[second].event = touches[index].current;
        touches[second].flags |= TOUCH_EVENT | TOUCH_TWO_TOUCH_EVENT;
        touches[index].flags |= TOUCH_TWO_TOUCH_EVENT;
        touches[second].event_time = time;
        if (event_settings[type].flags & SINGLE_FLAG)
            touches[second].flags |= TOUCH_IGNORE_EVENTS;
    }
}
