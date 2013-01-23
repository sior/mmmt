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
#include <string.h>
#include "touch.h"
#include "events.h"

#include <ncurses.h>

Finger* Touch2::getCurrentFinger()
{
    int i;
    for (i = 0; i < MAX_NUM_FINGERS; i++){
        if (fingers[i].slot == currentSlot){
            return &fingers[i];
        }
    }
    return NULL;
}

void Touch2::newFinger()
{
    if (getCurrentFinger() == NULL){
        int i;
        for (i = 0; i < MAX_NUM_FINGERS; i++){
            if (fingers[i].slot == -1){
                fingers[i].slot = currentSlot;
                setFlag(FINGER_FLAG_NEW);
                numberOfFingers++;
                if (numberOfFingers > MAX_NUM_FINGERS)numberOfFingers = MAX_NUM_FINGERS;
                fingers[i].id = idHolder;
                break;
            }
        }
    }
}

void Touch2::removeFinger()
{
    Finger *tmp = getCurrentFinger();
    if (!tmp)
        return;
    
    memset(tmp, 0, sizeof(Finger));
    tmp->slot = -1;
    tmp->id = -1;
    numberOfFingers--;
    if (numberOfFingers < 0)numberOfFingers = 0;
}

void Touch2::removeFinger(int index)
{
    memset(&fingers[index], 0, sizeof(Finger));
    fingers[index].slot = -1;
    fingers[index].id = -2;
    fingers[index].flags = 128;
    numberOfFingers--;
    if (numberOfFingers < 0)numberOfFingers = 0;
}

int Touch2::fingerActive(int index)
{
    return fingers[index].slot > -1;
}

void Touch2::setFlag(int flag)
{
    Finger *tmp = getCurrentFinger();
    if (tmp)
        tmp->flags |= flag;
}

void Touch2::setFlag(int flag, int index)
{
    fingers[index].flags |= flag;
}

void Touch2::clearFlag(int flag)
{
    Finger *tmp = getCurrentFinger();
    if (tmp)
        tmp->flags &= ~flag;
}

int Touch2::isFlagSet(int flag)
{
    Finger *tmp = getCurrentFinger();
    if (tmp)
        return tmp->flags & flag;
    return 0;
}

int Touch2::isFlagSet(int flag, int index)
{
    return fingers[index].flags & flag;
}

void Touch2::setX(int x)
{
    Finger *tmp = getCurrentFinger();
    if (!tmp)
        return;

    if (isFlagSet(FINGER_FLAG_X_SET)){
        tmp->currentX = x;
    } else {
        tmp->originX = x;
        tmp->currentX = x;
        tmp->eventX = x;
        setFlag(FINGER_FLAG_X_SET);
    }
}

void Touch2::setY(int y)
{
    Finger *tmp = getCurrentFinger();
    if (!tmp)
        return;
    
    if (isFlagSet(FINGER_FLAG_Y_SET)){
        tmp->currentY = y;
    } else {
        tmp->originY = y;
        tmp->currentY = y;
        tmp->eventY = y;
        setFlag(FINGER_FLAG_Y_SET);
    }

}

int Touch2::dx(int index)
{
    if (isFlagSet(FINGER_FLAG_EVENT, index))
        return fingers[index].currentX - fingers[index].eventX;
    else
        return fingers[index].currentX - fingers[index].originX;
}

int Touch2::dy(int index)
{
    if (isFlagSet(FINGER_FLAG_EVENT, index))
        return fingers[index].currentY - fingers[index].eventY;
    else
        return fingers[index].currentY - fingers[index].originY;
}

void Touch2::eventHere(int event, struct timeval time)
{
    int i;
    int isSingle = runEvent(event);
    for (i = 0; i < MAX_NUM_FINGERS; i++){
        if (fingers[i].slot != -1){
            setFlag(FINGER_FLAG_EVENT, i);
            fingers[i].eventX = fingers[i].currentX;
            fingers[i].eventY = fingers[i].currentY;
            fingers[i].eventTime = time;
            if (isSingle)
                setFlag(FINGER_FLAG_IGNORE, i);
        }
    }
}

int Touch2::getNumberOfFingers()
{
    return numberOfFingers;
}

void Touch2::init()
{
    initscr();
    refresh();
    memset(fingers, 0, MAX_NUM_FINGERS * sizeof(Finger));
    int i;
    for (i = 0; i < MAX_NUM_FINGERS; i++){
        fingers[i].id = -1;
        fingers[i].slot = -1;
    }
    numberOfFingers = 0;
    currentSlot = -1;
    testing = 0;
}

void Touch2::processEvent(struct input_event event)
{
    Finger *tmp = getCurrentFinger();
    if (event.type == 4){
        //process raw
        //if ((event.value & 0xf0) == 0x10){
         //   testing++;
        //}
        //0x20 finger near on new touch
        //0x40 finger down
        if ((event.value & 0xf0) == 0x40){
            newFinger();
        }
        //0x60 finger near on old touch
        //0x70 finger gone
        if ((event.value & 0xf0) == 0x70){
            removeFinger();
        } else {
            if (!tmp)return;
            tmp->unknown = event.value;
        }
    } else if (event.type == 3){
        //process abs
        if (event.code == 47){
            //change current slot
            currentSlot = event.value;
        } else if (event.code == 48){
            //Major axis
            if (!tmp)return;
            tmp->major = event.value;
        } else if (event.code == 49){
            //Minor axis
            if (!tmp)return;
            tmp->minor = event.value;
        } else if (event.code == 52){
            //Orientation
            if (!tmp)return;
            tmp->orientation = event.value;
        } else if (event.code == 53){
            //X
            setX(event.value);
        } else if (event.code == 54){
            //Y
            setY(event.value);
        } else if (event.code == 57){
            //id
            move(0, 70);
            printw("%d\n\n", event.value);
            refresh();
            idHolder = event.value;
            if (event.value == -1){
                move(1, 70);
                printw("Kill slot %d\t", currentSlot);
                removeFinger(currentSlot);
                refresh();
            }
            if (!tmp)return;
            tmp->id = event.value;
        }
    }
    checkTwoButtonClick(numberOfFingers);
}

void Touch2::processFingers(struct timeval time)
{
    int i;

    int fingersSwipingUp = 0;
    int fingersSwipingDown = 0;
    int fingersSwipingLeft = 0;
    int fingersSwipingRight = 0;

    for (i = 0; i < MAX_NUM_FINGERS; i++){
        if (!fingerActive(i))
            continue;
        if (isFlagSet(FINGER_FLAG_IGNORE, i))
            continue;
        if (fingers[i].id < 0){
            removeFinger();
            continue;
        }
        if (dx(i) > SWIPE_THRESHOLD)
            fingersSwipingRight++;
        if (dx(i) < -SWIPE_THRESHOLD)
            fingersSwipingLeft++;
        if (dy(i) > SWIPE_THRESHOLD)
            fingersSwipingDown++;
        if (dy(i) < -SWIPE_THRESHOLD)
            fingersSwipingUp++;
    }

    if ((fingersSwipingRight) && (fingersSwipingRight == numberOfFingers)){
        if (numberOfFingers == 1)
            eventHere(EVENT_SWIPE_RIGHT_ONE, time);
        else if (numberOfFingers == 2)
            eventHere(EVENT_SWIPE_RIGHT_TWO, time);
    }

    if ((fingersSwipingLeft) && (fingersSwipingLeft == numberOfFingers)){
        if (numberOfFingers == 1)
            eventHere(EVENT_SWIPE_LEFT_ONE, time);
        else if (numberOfFingers == 2)
            eventHere(EVENT_SWIPE_LEFT_TWO, time);
    }

    if ((fingersSwipingDown) && (fingersSwipingDown == numberOfFingers)){
        if (numberOfFingers == 1)
            eventHere(EVENT_SWIPE_DOWN_ONE, time);
        else if (numberOfFingers == 2)
            eventHere(EVENT_SWIPE_DOWN_TWO, time);
    }

    if ((fingersSwipingUp) && (fingersSwipingUp == numberOfFingers)){
        if (numberOfFingers == 1)
            eventHere(EVENT_SWIPE_UP_ONE, time);
        else if (numberOfFingers == 2)
            eventHere(EVENT_SWIPE_UP_TWO, time);
    }
}

void Touch2::displayDebug()
{
    int i;
    for (i = 0; i < MAX_NUM_FINGERS; i++){
        move(0,i*15);
        printw("Finger%d\t", i);
        move(1,i*15);
        printw("slot:%d\t", fingers[i].slot);
        move(2,i*15);
        printw("id:%d\t", fingers[i].id);
        move(3,i*15);
        printw("dy:%d\t", dy(i));
        move(4,i*15);
        printw("flags:%d \t", fingers[i].flags);
        move(5,i*15);
        printw("unknown:%d \t", fingers[i].unknown);
        move(6,i*15);
        printw("major:%d \t", fingers[i].major);
        move(7,i*15);
        printw("minor:%d \t", fingers[i].minor);
    }
    
    move(15,0);
    printw("numberOfFingers:%d\t", numberOfFingers);
    move(16,0);
    printw("%d\t", testing);
    refresh();
}
