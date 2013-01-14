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

#ifndef TOUCHH
#define TOUCHH
#include <linux/input.h>
#include <list>

#define MAX_SLOTS 25      //I think this is actually 15, but added some to be sure
#define MAX_NUM_TOUCHES 4 //max number of touches we will track
                           
#define TOUCH_NEW  1
#define TOUCH_INITED  2
#define TOUCH_EVENT  4
#define TOUCH_ENDED  8
#define TOUCH_IGNORE_EVENTS  16
#define TOUCH_FIRST  32
#define TOUCH_SECOND  64
#define TOUCH_THIRD  128
#define TOUCH_FOURTH  256
#define TOUCH_TWO_TOUCH_EVENT  512
#define TOUCH_ORIGIN_X_SET  1024
#define TOUCH_ORIGIN_Y_SET  2048

#define SWIPE_THRESHOLD 150

class Touch{
    private:
        int id;
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
        struct timeval originTime;
        struct timeval eventTime;
        int unknown;
        int eventFlags;

    public:
        Touch();
        
        void setId(int value);
        void setMajor(int value);
        void setMinor(int value);
        void setOrientation(int value);
        void setOriginX(int value);
        void setOriginY(int value);
        void setEventX(int value);
        void setEventY(int value);
        void setCurrentX(int value);
        void setCurrentY(int value);
        void setFlags(int value);
        void setOriginTime(struct timeval value);
        void setEventTime(struct timeval value);
        void setUnknown(int value);
        
        int getId();
        int getMajor();
        int getMinor();
        int getOrientation();
        int getOriginX();
        int getOriginY();
        int getEventX();
        int getEventY();
        int getCurrentX();
        int getCurrentY();
        int getFlags();
        struct timeval getOriginTime();
        struct timeval getEventTime();
        int getUnknown();

        int isFlagSet(int flag);
        void clearFlags();
        void clearFlag(int flag);
        void setFlag(int flag);
        
        void setX(int x);
        void setY(int y);
        
        int dx();
        int dy();

        void clearData();

        void eventHere(struct timeval time);
};

void initTouches();
void processReport(std::list<struct input_event> report);
void processTouches(struct timeval time);
#endif
