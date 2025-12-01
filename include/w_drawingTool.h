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

#include "w_types.h"

// extern char *COLOR_ARRAY[];
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