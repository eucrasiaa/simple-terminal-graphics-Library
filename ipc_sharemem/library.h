#ifndef LIBR_H
#define LIBR_H

#include <stdint.h>   // uint32_t + uint8_t
#include <stddef.h>   // size_t
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


//typedef for struct for shared memory
#define DEBUGKEY 1111
#define SHM_SIZE 1024 * 1024  // 1 MB shared memory segment size
//demo structs to test shared memory
typedef struct w_frame {
    uint32_t ch;
    uint8_t color;
    uint8_t flags;  // bit0 = use_color, bit1 = reset_after, other bits reserved
} w_frame_t;


// framebuffer: a 1D array of w_frame_t elements, logically rows x cols
typedef struct w_framebuffer {
    w_frame_t *cells;     // malloc(rows * cols * sizeof(w_frame_t))
} w_framebuffer_t;

typedef struct w_window{
    int id; // just for sanity
    unsigned int rows; // height
    unsigned int cols; // width
    unsigned int x; // cursor position?
    unsigned int y; // see above 
    char title[24]; // maybe?
    int active; // 1 active, 0 inactive 
    w_framebuffer_t fb; // long 1d array, divided into cols and rows on print
} w_window_t;


typedef struct w_windowManager{
  // stores a single window, position, and z-order
  w_window_t *aWindow; // array of pointers to windows
  unsigned int x; // position on screen
  unsigned int y; // position on screen
  unsigned int zOrder; // higher zOrder means drawn on top
} w_windowManager_t;


// screenbuffer will store windowManager elems via a linkedlist, so define 
typedef struct w_windowManagerNode{
    w_windowManager_t * windowMgr;
    struct w_windowManagerNode* next;
} w_windowManagerNode_t; 
// handler functions for linkedlist:
// double or single linked? 


typedef struct screenBuffer{
    unsigned int rows; // terminal height
    unsigned int cols; // terminal width
    // array of window managers
    // w_windowManager_t *windows; // array of window managers
    w_windowManagerNode_t* head; // linkedlist of window managers
    unsigned int numWindows; // number of windows being managed
} screenBuffer_t;

// dead simple functions to create one of the structs and return pointer
// populate with just random vals to test
w_window_t * makeWin(unsigned int rows, unsigned int cols);
w_windowManager_t * makeWinMgr(w_window_t *win, unsigned int zOrder);
int addNewNode(screenBuffer_t *screen, w_windowManager_t *wm);

screenBuffer_t* makeSB_empty(unsigned int termRows, unsigned int termCols);
screenBuffer_t * makeSB_randomNode(unsigned int termRows, unsigned int termCols);
screenBuffer_t * makeSB_twoNode(unsigned int termRows, unsigned int termCols);

void printW_info(w_window_t *win);
void printWM_info(w_windowManager_t *wm);
void printSB_info(screenBuffer_t *sb);

#endif //LIBR_H