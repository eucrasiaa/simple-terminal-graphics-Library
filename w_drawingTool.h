#ifndef W_DRAWINGTOOL_H
#define W_DRAWINGTOOL_H
/*
 * as usual, handwriten by yours truly
 * created by Will C. (eucrasiaa)
 * last update: 29-11-2025
 * a barebones terminal drawing tool, supporting windows, colors, UTF-8 (painstakingly), etc
 */
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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

#define COLOR_COUNT 9
// extern char *COLOR_ARRAY[];


enum { TOPLEFT=0, TOPRIGHT=1, BOTTOMLEFT=2, BOTTOMRIGHT=3,
     HORIZONTAL=4, VERTICAL=5, TEE_TOP=6, TEE_BOTTOM=7,
     TEE_LEFT=8, TEE_RIGHT=9, TEE_LR=10, TEE_BOTTOM_LR=11 };

enum LINEMODES_ENUM { SINGLE=0, HEAVY=1, DOUBLE=2, DASHED=3,ROUNDED=100};

#define NUM_CHARS 12

static const uint32_t SINGLE_LINE[NUM_CHARS] = {
   0x250C, 0x2510, 0x2514, 0x2518, 0x2500, 0x2502,
   0x252C, 0x2534, 0x251C, 0x2524, 0x253C, 0x253C
};

static const uint32_t HEAVY_LINE[NUM_CHARS] = {
   0x250F, 0x2513, 0x2517, 0x251B, 0x2501, 0x2503,
   0x252F, 0x2537, 0x2523, 0x252B, 0x254B, 0x254B
};

static const uint32_t DOUBLE_LINE[NUM_CHARS] = {
   0x2554, 0x2557, 0x255A, 0x255D, 0x2550, 0x2551,
   0x2566, 0x2569, 0x2560, 0x2563, 0x256C, 0x256C
};

static const uint32_t DASHED_LINE[NUM_CHARS] = {
   0x250C, 0x2510, 0x2514, 0x2518, 0x2504, 0x2506,
   0x252C, 0x2534, 0x251C, 0x2524, 0x253C, 0x253C
};

static const uint32_t ROUNDED_CORNERS[4] = {
   0x256D, 0x256E, 0x2570, 0x256F /* TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT */
};

static const uint32_t * const LINEMODES[4] = {
   SINGLE_LINE, HEAVY_LINE, DOUBLE_LINE, DASHED_LINE
};
/*
*  0 = single line
*  1 = heavy line
*  2 = double line
*  3 = dashed line
*/

// colors are 7 bytes and A_RESET is 4 bytes, so 

// single cell/frame: a character plus optional color/reset info
typedef struct w_frame {
    uint32_t ch;    // Unicode codepoint (0 for empty). Use uint32_t to store UTF-32 codepoints.
    uint8_t color;  // index into COLOR_ARRAY[] (0 == A_RESET). Use 0..(COLOR_COUNT-1)
    uint8_t flags;  // bit0 = use_color, bit1 = reset_after, other bits reserved
    // uint8_t _pad;   // padding to keep the struct aligned (total size = 8 bytes)
} w_frame_t;

#define WF_USE_COLOR  0x01
#define WF_RESET_AFTER 0x02


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
/** Function to draw box characters around edges of a window
 *
 * 
 * @param win pointer to window to box
 * @param mode variety of box styles exist:

 */

 /* mode determined by 2 fields:
  * because single & dashed lines can support both regular & round corners, bit mask or 
 line type
 *  0 = single line
 *  1 = heavy line
 *  2 = double line
 *  3 = dashed line
 corner type
    *  0 = standard corners //default
    *  1 = rounded corners

    00 00 = default corners, single line
    01 00 = rounded corners, single line
    00 01 = default corners, heavy line
    etc
 */ 
// char modeGen(int linetype, int cornertype) {
//     if (linetype < 0 || linetype > 3 || cornertype < 0 || cornertype > 1) {
//         return -1; // invalid mode
//     }
//     return (char)( (cornertype << 2) | linetype );
// }


screenBuffer_t * initWDrawTool(unsigned int termRows, unsigned int termCols);
w_window_t *w_createWindow(unsigned int rows, unsigned int cols, unsigned int x, unsigned int y, const char* title, int active);



// perhaps have draw function int returns be used to track the loaded window in a screen buffer, so we can manage them later? 
// draw functions load into the drawing buffer for the given screen
int w_draw(screenBuffer_t *screen, const w_window_t *win);
// draw functions load into the drawing buffer for the given screen
int w_drawAt(screenBuffer_t *screen, const w_window_t *win, unsigned int x, unsigned int y);
int deleteWindow(w_window_t *win);
// if int is negative, rounded corners used
int w_boxAWindow(w_window_t *win, int mode);
int wSetTermClear();
int wRenderScreen(screenBuffer_t *screen);
int clearDrawBuff(screenBuffer_t *screen);
int deleteScreen(screenBuffer_t *screen);

int putRawChar(uint32_t ch, uint8_t color);
int putFrame(w_frame_t printMe);
int fillWithChar(w_window_t *win, uint32_t ch);
int fillWithFrame(w_window_t *win, const w_frame_t frame);
/*
    U+250x	─	━	│	┃	┄	┅	┆	┇	┈	┉	┊	┋	┌	┍	┎	┏
    U+251x	┐	┑	┒	┓	└	┕	┖	┗	┘	┙	┚	┛	├	┝	┞	┟
    U+252x	┠	┡	┢	┣	┤	┥	┦	┧	┨	┩	┪	┫	┬	┭	┮	┯
    U+253x	┰	┱	┲	┳	┴	┵	┶	┷	┸	┹	┺	┻	┼	┽	┾	┿
    U+254x	╀	╁	╂	╃	╄	╅	╆	╇	╈	╉	╊	╋	╌	╍	╎	╏
    U+255x	═	║	╒	╓	╔	╕	╖	╗	╘	╙	╚	╛	╜	╝	╞	╟
    U+256x	╠	╡	╢	╣	╤	╥	╦	╧	╨	╩	╪	╫	╬	╭	╮	╯
    U+257x	╰	╱	╲	╳	╴	╵	╶	╷	╸	╹	╺	╻	╼	╽	╾	╿

 *  0 = single line
 *  1 = heavy line
 *  2 = double line
 *  3 = dashed line
0:
─ │
┌ ┐
└ ┘
1:
━ ┃
┏ ┓
┗ ┛
2:
═ ║
╔ ╗
╚ ╝
3:
┄ ┆
┌ ┐
└ ┘
  ┬
├ ┼ ┤
  ┴
╭ ╮ 
╰ ╯
*/
// prevent multiple inclusions
#endif // W_DRAWINGTOOL_H