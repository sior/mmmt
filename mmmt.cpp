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

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <errno.h>
#include <list>
#include "touch.h"
#include "events.h"

#define ID_APPLE             0x05ac
#define ID_APPLE_MAGICMOUSE  0x030d

//std::list<struct input_event> reportList;

//Get the file for the magic mouse events.  Loop until the mouse is discovered
int getFd(){
    int i, fd;
    char dirname[] = "/dev/input/";
    char path[20] = "/dev/input/event";
    struct dirent *dptr;
    DIR *dirp;
    do {
        if ((dirp = opendir(dirname)) == NULL){
            perror("dir open");
        }
        while ((dptr = readdir(dirp))){
            if (strstr(dptr->d_name, "event") != NULL){
                int l = strlen(dptr->d_name) - 5;
                for (i = 0; i < l; i++){
                    path[16 + i] = dptr->d_name[i + 5];
                }
                path[16 + l] = '\0';
                fd = open(path, O_RDONLY | O_NONBLOCK );
                struct input_id device_info;
                ioctl(fd, EVIOCGID, &device_info);
                if ((device_info.vendor == ID_APPLE) && 
                    (device_info.product == ID_APPLE_MAGICMOUSE))
                        break;
                close(fd);
                fd = -1;
            }
        }
        sleep(1);
        closedir(dirp);
    } while (fd == -1);
    return fd;
}

int main ( int argc, char *argv[] ){
while(1){
    getDisplay();
    int fd = getFd();

    Touch2 touch;
    touch.init();
    initEvents();
    fd_set set;
    int i;
    size_t bytesRead;
    struct timeval timeout;
    
    struct input_event events[64];

    while (1){
        FD_ZERO(&set);
        FD_SET(fd, &set);

        timeout.tv_usec = 300;
        timeout.tv_sec = 0;

        if (select(FD_SETSIZE, &set, NULL, NULL, &timeout) > 0){
            bytesRead = read(fd, events, sizeof(struct input_event));
            if (bytesRead < (int)sizeof(struct input_event)){
                perror("short event read error");
                return 1;
            }

            if (errno == ENODEV){
                
                close(fd);
                break;
                //fd = getFd();
                //touch.init();
                //continue;
            }
            checkTwoButtonClick(touch.getNumberOfFingers());

            for (i = 0; i < (int)(bytesRead/sizeof(struct input_event)); i++){
                if (events[i].type == 3){
                    //process mt event
                    touch.processEvent(events[i]);
                } else if (events[i].type == 4){
                    //process misc event
                    if (events[i].code == 3){
                        touch.processEvent(events[i]);
                    }
                } else if (events[i].type == 0){
                    //process syn event
                    touch.processFingers(events[i].time);
                } else if (events[i].type == 1){
                    //process button event
                    updateButtonState(events[i].code, events[i].value, &touch); 
                    checkTwoButtonClick(touch.getNumberOfFingers());
                }
            }
        }
    }
}
    return 0;
}
