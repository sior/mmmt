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

#ifndef TOUCH2H
#define TOUCH2H

#include <linux/input.h>

#define MAX_NUM_FINGERS 4

#define FINGER_FLAG_X_SET 1
#define FINGER_FLAG_Y_SET 2
#define FINGER_FLAG_NEW 4
#define FINGER_FLAG_EVENT 8
#define FINGER_FLAG_SINGLE 16
#define FINGER_FLAG_IGNORE 32
#define FINGER_FLAG_MAJOR_SET 64
#define FINGER_FLAG_CLICKING 128
#define FINGER_FLAG_IS_DOWN 256

#define SWIPE_THRESHOLD 150

#define FINGER_TIMEOUT 200

typedef struct {
    int id;
    int slot;
    int major;
    int minor;
    int orientation;
    int originX;
    int originY;
    int eventX;
    int eventY;
    int currentX;
    int currentY;
    int flags;
    int unknown;
    struct timeval originTime;
    struct timeval eventTime;
    struct timeval lastTime;
} Finger;

class Touch2{
    private:
        Finger fingers[MAX_NUM_FINGERS];
        int currentSlot;
        int testing;
        int idHolder;
        struct timeval currentEventTime;

        Finger* getCurrentFinger();
        
        void newFinger();
        void removeFinger();
        void removeFinger(int index);

        int fingerActive(int index);

        void setFlag(int flag);
        void setFlag(int flag, int index);
        void clearFlag(int flag);
        void clearFlag(int flag, int index);
        int isFlagSet(int flag);
        int isFlagSet(int flag, int index);

        void setX(int x);
        void setY(int y);
        void setMajor(int major);
        void setMinor(int minor);
        void setOrientation(int orientation);
        void setUnknown(int unknown);
        void setId(int id);

        int dx(int index);
        int dy(int index);
        
        int timeOut(int index);

        void eventHere(int event, struct timeval time);
    public:
        int getNumberOfFingers();

        void init();
        
        void processEvent(struct input_event event);

        void processFingers(struct timeval time);
        
        void fingerClicking();
        void fingerNotClicking();

        void displayDebug();
};

#endif
