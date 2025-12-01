#ifndef W_TYPES_H
#define W_TYPES_H

#include <stdint.h>   // uint32_t + uint8_t
#include <stddef.h>   // size_t

// ~~~~~~ Term Color Codes ~~~~~~
#define A_ESC "\033["
#define A_RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define BLACK "\033[1;30m"
// colors are 7 bytes and A_RESET is 4 bytes, so 
#define COLOR_COUNT 9
extern const char *COLOR_ARRAY[COLOR_COUNT];


// ~~~~~~ Box-Draw Constants ~~~~~~
enum { TOPLEFT=0, TOPRIGHT=1, BOTTOMLEFT=2, BOTTOMRIGHT=3,
     HORIZONTAL=4, VERTICAL=5, TEE_TOP=6, TEE_BOTTOM=7,
     TEE_LEFT=8, TEE_RIGHT=9, TEE_LR=10, TEE_BOTTOM_LR=11 };
enum LINEMODES_ENUM { SINGLE=0, HEAVY=1, DOUBLE=2, DASHED=3,ROUNDED=100};
#define NUM_CHARS 12


extern const uint32_t SINGLE_LINE[NUM_CHARS];// = {
//    0x250C, 0x2510, 0x2514, 0x2518, 0x2500, 0x2502,
//    0x252C, 0x2534, 0x251C, 0x2524, 0x253C, 0x253C
// };
extern const uint32_t HEAVY_LINE[NUM_CHARS];// = {
//    0x250F, 0x2513, 0x2517, 0x251B, 0x2501, 0x2503,
//    0x252F, 0x2537, 0x2523, 0x252B, 0x254B, 0x254B
// };
extern const uint32_t DOUBLE_LINE[NUM_CHARS];// = {
//    0x2554, 0x2557, 0x255A, 0x255D, 0x2550, 0x2551,
//    0x2566, 0x2569, 0x2560, 0x2563, 0x256C, 0x256C
// };
extern const uint32_t DASHED_LINE[NUM_CHARS];// = {
//     0x250C, 0x2510, 0x2514, 0x2518, 0x2504, 0x2506,
//     0x252C, 0x2534, 0x251C, 0x2524, 0x253C, 0x253C
// };
extern const uint32_t ROUNDED_CORNERS[4];// = {
//    0x256D, 0x256E, 0x2570, 0x256F /* TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT */
// };
extern const uint32_t * const LINEMODES[4];// = {
//    SINGLE_LINE, HEAVY_LINE, DOUBLE_LINE, DASHED_LINE
// };
/*
*  0 = single line
*  1 = heavy line
*  2 = double line
*  3 = dashed line
*/
// ~~~~~~ Main Structs ~~~~~~
// single cell/frame: a character plus optional color/reset info
typedef struct w_frame {
    uint32_t ch;    // Unicode codepoint (0 for empty). Use uint32_t to store UTF-32 codepoints.
    uint8_t color;  // index into COLOR_ARRAY[] (0 == A_RESET). Use 0..(COLOR_COUNT-1)
    uint8_t flags;  // bit0 = use_color, bit1 = reset_after, other bits reserved
    // uint8_t _pad;   // padding to keep the struct aligned (total size = 8 bytes)
} w_frame_t;

// ~~ Frame Flags ~~
#define WF_USE_COLOR  0x01
#define WF_RESET_AFTER 0x02
// more added here as needed



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



#endif // W_TYPES_H