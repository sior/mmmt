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

#include "touch.h"
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include "events.h"

int currentTouchSlot;
int numberOfTouches;
Touch touches[MAX_SLOTS];
int activeSlots[MAX_NUM_TOUCHES];

static void triggerEvents(struct timeval time, int type);
static void activateSlot(int slot);
static void deactivateSlot(int slot);
static int isSlotActive(int slot);

Touch::Touch(){
    id = 0;
    major = 0;
    minor = 0;
    orientation = 0;
    originX = 0;
    originY = 0;
    eventX = 0;
    eventY = 0;
    currentX = 0;
    currentY = 0;
    flags = 0;
    originTime.tv_usec = 0;
    originTime.tv_sec = 0;
    eventTime.tv_usec = 0;
    eventTime.tv_sec = 0;
    unknown = 0;
}
void Touch::setId(int value){id = value;}
void Touch::setMajor(int value){major = value;}
void Touch::setMinor(int value){minor = value;}
void Touch::setOrientation(int value){orientation = value;}
void Touch::setOriginX(int value){originX = value;}
void Touch::setOriginY(int value){originY = value;}
void Touch::setEventX(int value){eventX = value;}
void Touch::setEventY(int value){eventY = value;}
void Touch::setCurrentX(int value){currentX = value;}
void Touch::setCurrentY(int value){currentY = value;}
void Touch::setFlags(int value){flags = value;}
void Touch::setOriginTime(struct timeval value){originTime = value;}
void Touch::setEventTime(struct timeval value){eventTime = value;}
void Touch::setUnknown(int value){unknown = value;}
int Touch::getId(){return id;}
int Touch::getMajor(){return major;}
int Touch::getMinor(){return minor;}
int Touch::getOrientation(){return orientation;}
int Touch::getOriginX(){return originX;}
int Touch::getOriginY(){return originY;}
int Touch::getEventX(){return eventX;}
int Touch::getEventY(){return eventY;}
int Touch::getCurrentX(){return currentX;}
int Touch::getCurrentY(){return currentY;}
int Touch::getFlags(){return flags;}
struct timeval Touch::getOriginTime(){return originTime;}
struct timeval Touch::getEventTime(){return eventTime;}
int Touch::getUnknown(){return unknown;}


int Touch::isFlagSet(int flag)
{
    return flags & flag;
}

void Touch::clearFlags()
{
    flags = 0;
}

void Touch::clearFlag(int flag)
{
    flags &= ~flag;
}

void Touch::setFlag(int flag)
{
    flags |= flag;
}

void Touch::setX(int x)
{
    if (isFlagSet(TOUCH_ORIGIN_X_SET)){
        currentX = x;
    } else {
        originX = currentX = eventX = x;
        setFlag(TOUCH_ORIGIN_X_SET);
    }
}

void Touch::setY(int y)
{
    if (isFlagSet(TOUCH_ORIGIN_Y_SET)){
        currentY = y;
    } else {
        originY = currentY = eventY = y;
        setFlag(TOUCH_ORIGIN_Y_SET);
    }
}

int Touch::dx()
{
    if (isFlagSet(TOUCH_EVENT)){
        return currentX - eventX;
    } else {
        return currentX - originX;
    }
}

int Touch::dy()
{
    if (isFlagSet(TOUCH_EVENT)){
        return currentY - eventY;
    } else {
        return currentY - originY;
    }
}

void Touch::clearData()
{
    id = 0;
    major = 0;
    minor = 0;
    orientation = 0;
    originX = 0;
    originY = 0;
    eventX = 0;
    eventY = 0;
    currentX = 0;
    currentY = 0;
    flags = 0;
    originTime.tv_usec = 0;
    originTime.tv_sec = 0;
    eventTime.tv_usec = 0;
    eventTime.tv_sec = 0;
    unknown = 0;
}

void Touch::eventHere(struct timeval time)
{
    eventX = currentX;
    eventY = currentY;
    eventTime = time;
    setFlag(TOUCH_EVENT);
}

void initTouches()
{
    currentTouchSlot = -1;
    numberOfTouches = 0;
    memset(activeSlots, -1, sizeof(int) * MAX_NUM_TOUCHES);
    initEvents();
}

void processReport(std::list<struct input_event> report)
{
    std::list<struct input_event>::iterator i;

    for (i = report.begin(); i != report.end(); i++){
        if (i->type == 4){ 
            //process raw
            if (i->code == 3){
                touches[currentTouchSlot].setUnknown(i->value);
            }
        } else if (i->type == 3){
            //process abs
            if (i->code == 47){
                currentTouchSlot = i->value;
            } else if (i->code == 48){
                touches[currentTouchSlot].setMajor(i->value);
            } else if (i->code == 49){
                touches[currentTouchSlot].setMinor(i->value);
            } else if (i->code == 52){
                touches[currentTouchSlot].setOrientation(i->value);
            } else if (i->code == 53){
                touches[currentTouchSlot].setX(i->value);
            } else if (i->code == 54){
                touches[currentTouchSlot].setY(i->value);
            } else if (i->code == 57){
                if (i->value < 0){
                    //if (touches[currentTouchSlot].getId() != -1){
                        numberOfTouches--;
                        if (numberOfTouches < 0)numberOfTouches=0; 
                        deactivateSlot(currentTouchSlot);
                        touches[currentTouchSlot].clearData();
                        touches[currentTouchSlot].setId(i->value);
                    //}
                } else {
                    touches[currentTouchSlot].setId(i->value);
                    numberOfTouches++;
                    if (numberOfTouches > MAX_NUM_TOUCHES)numberOfTouches = MAX_NUM_TOUCHES;
                    touches[currentTouchSlot].clearData();
                    touches[currentTouchSlot].setFlag(TOUCH_NEW);
                    activateSlot(currentTouchSlot);
                }
            }
        }
    }
}

void processTouches(struct timeval time)
{
    int i, slot;
    int as = 0;
    
    int fingersSwipingUp = 0;
    int fingersSwipingDown = 0;
    int fingersSwipingLeft = 0;
    int fingersSwipingRight = 0;
    
    for (i = 0; i < MAX_NUM_TOUCHES; i++){
        if (!isSlotActive(i))
            continue;
        as++;
        slot = activeSlots[i];
        if (touches[slot].isFlagSet(TOUCH_IGNORE_EVENTS))
            continue;
        if (touches[slot].isFlagSet(TOUCH_NEW)){
            touches[slot].clearFlag(TOUCH_NEW);
            touches[slot].setOriginTime(time);
        }
        
        if (touches[slot].dx() > SWIPE_THRESHOLD){
            fingersSwipingRight++;
        }
        if (touches[slot].dx() < -SWIPE_THRESHOLD){
            fingersSwipingLeft++;
        }
        if (touches[slot].dy() > SWIPE_THRESHOLD){
            fingersSwipingDown++;
        }
        if (touches[slot].dy() < -SWIPE_THRESHOLD){
            fingersSwipingUp++;
        }
    }
    if (fingersSwipingUp){
        if (fingersSwipingUp == numberOfTouches){
            if (numberOfTouches == 1)
                triggerEvents(time, EVENT_SWIPE_UP_ONE);
            if (numberOfTouches == 2)
                triggerEvents(time, EVENT_SWIPE_UP_TWO);
        }
    }
    
    if (fingersSwipingDown){
        if (fingersSwipingDown == numberOfTouches){
            if (numberOfTouches == 1)
                triggerEvents(time, EVENT_SWIPE_DOWN_ONE);
            if (numberOfTouches == 2)
                triggerEvents(time, EVENT_SWIPE_DOWN_TWO);
        }
    }
    
    if (fingersSwipingLeft){
        if (fingersSwipingLeft == numberOfTouches){
            if (numberOfTouches == 1)
                triggerEvents(time, EVENT_SWIPE_LEFT_ONE);
            if (numberOfTouches == 2)
                triggerEvents(time, EVENT_SWIPE_LEFT_TWO);
        }
    }
    
    if (fingersSwipingRight){
        if (fingersSwipingRight == numberOfTouches){
            if (numberOfTouches == 1)
                triggerEvents(time, EVENT_SWIPE_RIGHT_ONE);
            if (numberOfTouches == 2)
                triggerEvents(time, EVENT_SWIPE_RIGHT_TWO);
        }
    }
}

static void triggerEvents(struct timeval time, int type)
{
    int i;
    int isSingle = runEvent(type);
    for (i = 0; i < MAX_NUM_TOUCHES; i++){
        if (activeSlots[i] != -1){
            touches[activeSlots[i]].eventHere(time);
            if (isSingle)
                touches[activeSlots[i]].setFlag(TOUCH_IGNORE_EVENTS);
        }
    }
}

static void activateSlot(int slot)
{
    int i;
    for (i = 0; i < MAX_NUM_TOUCHES; i++){
        if (activeSlots[i] == -1){
            activeSlots[i] = slot;
            return;
        }
    }
}

static void deactivateSlot(int slot)
{
    int i;
    for (i = 0; i < MAX_NUM_TOUCHES; i++){
        if (activeSlots[i] == slot){
            activeSlots[i] = -1;
            touches[slot].clearData();
            return;
        }
    }
}

static int isSlotActive(int slot)
{
    return activeSlots[slot] == -1 ? 0 : 1;
}
