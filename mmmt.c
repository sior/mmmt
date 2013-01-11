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
#include <linux/input.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <dirent.h>
#include <errno.h>
#include "touch.h"
#include "util.h"
#include "events.h"

#define ID_APPLE             0x05ac
#define ID_APPLE_MAGICMOUSE  0x030d

void process_event(int code, int value, struct timeval time){
    int index;
    switch (code){
        case 0:
            printf("touch_process\n");
            touch_process(current_touch, time);
            break;
        case 3:
            touches[current_touch].unknown = value;
            break;
        case 47:
            current_touch = value;
            break;
        case 48:
            touches[current_touch].major = value;
            break;
        case 49:
            touches[current_touch].minor = value;
            break;
        case 52:
            touches[current_touch].orientation = value;
            break;
        case 53:
            //x
            touch_add_x(current_touch, value);
            break;
        case 54:
            //y
            touch_add_y(current_touch, value);
            break;
        case 57:
            if (value > -1){
                touch_new(current_touch);
                touches[current_touch].id = value;
            } else {
                check_for_tap(current_touch, time);
                touch_remove(current_touch);
                touches[current_touch].id = value;
            }
            break;
    }
    if (current_touch >= 0)
        touch_process(current_touch, time);
}

int get_fd(){
    int i, fd;
    char dirname[] = "/dev/input/";
    char path[20] = "/dev/input/event";
    struct dirent *dptr;
    DIR *dirp;
    do {
        if ((dirp = opendir(dirname)) == NULL){
            perror("dir open");
        }
        while (dptr = readdir(dirp)){
            if (strstr(dptr->d_name, "event") != NULL){
                int l = strlen(dptr->d_name) - 5;
                for (i = 0; i < l; i++){
                    path[16 + i] = dptr->d_name[i + 5];
                }
                path[16 + l] = '\0';
                fd = open(path, O_RDWR);
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

int init(){
    int i;
    
    current_touch = -1;
    num_touches = 0;
    touch_one_index = -1;
    touch_two_index = -1;

    for (i = 0; i < NUM_TOUCH_SLOTS; i++){
        touch_data_clear(i);
    }

    dpy = XOpenDisplay(NULL);
    
    load_settings();
    
    clear_tapping();

    return get_fd();
}

int main ( int argc, char *argv[] ){
    int version, fd, i, ready;
    fd_set set;
    struct input_event ev[64];
    size_t rb;
    struct timeval timeout;
    if ((fd = init()) < 0){
        perror("bad init");
    }

    printf("ready\n");
for(;;){
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_usec = 100;
    timeout.tv_sec = 0;

    tap_timeouts();
    
    if (select(FD_SETSIZE, &set, NULL, NULL, &timeout) > 0){
        rb = read(fd, ev, sizeof(struct input_event) * 64);
        if (rb < (int) sizeof(struct input_event)){
            perror("event read error short");
            exit(1);        
        }

        if (rb > (int)(sizeof(struct input_event) * 65)){
            if (errno == ENODEV){
                printf("reset\n");
                close(fd);
                fd = get_fd();
                continue;
            }
        }
    
        for (i = 0; i < (int) (rb / sizeof(struct input_event)); i++){
            if ((ev[i].type == 3) || (ev[i].type == 4))
                process_event(ev[i].code, ev[i].value, ev[i].time);
        }
    }
}

    close(fd);
    return 0;
}
