#include "w_drawingTool.h"
/*
 * as usual, handwriten by yours truly
 * created by Will C. (eucrasiaa)
 * last update: 29-11-2025
 * a barebones terminal drawing tool, supporting windows, colors, UTF-8 (painstakingly), etc
 */

char *COLOR_ARRAY[] = {
    A_RESET, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, BLACK
};



int debug_connect(){
    debug_fd = socket(AF_UNIX, SOCK_STREAM, 0); // local socket
    // using a UNIX domain socket, so sys/un.h gives the struct we need 
    // sockaddr_un:
    //  sun_family: address family (AF_UNIX)
    //  char sun_path[]: null-terminated pathname
    // so we fill out the info and then pass it to the connect()
    struct sockaddr_un addr; 
    memset(&addr, 0, sizeof(addr)); // clear it out
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, MY_SOCKET_PATH, sizeof(addr.sun_path) - 1);
    // why does connect fail? its because the socket file doesnt exist yet ig
    if (connect(debug_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect failed");
        return -1;
    }
    return debug_fd;
}
// testing if it even works LOL with msgs
void debug_send(const char *msg){
  if(debug_fd != -1){
    // send(debug_fd, msg, strlen(msg), 0);
    // send vs write?
    write(debug_fd, msg, strlen(msg));
  }
}

void debug_close(){
  if(debug_fd != -1){
    debug_send("<CMD><BREAK>");
    close(debug_fd);
    debug_fd = -1;
  }
}

int deleteWindow(w_window_t *win){
    if (win == NULL) {
        return -1; // error: null pointer
    }
    if (win->fb.cells != NULL) {
        free(win->fb.cells);
        win->fb.cells = NULL;
    }
    free(win); // why does this say "implicit declaration of function 'free
    return 0; // success
}

// quick linkedlist management functions:
int addNewNode(screenBuffer_t *screen, w_windowManager_t *wm) {
  // passed pointer is already malloc'd
    if (screen == NULL || wm == NULL) return -1;
    w_windowManagerNode_t *newNode = malloc(sizeof(w_windowManagerNode_t));
    if (newNode == NULL) return -1;
    // inserts at head, 
    newNode->windowMgr = wm;
    newNode->next = screen->head;
    screen->head = newNode;
    screen->numWindows++;
    return 0;
}
// skip over remove


// deletion steps:
// deleteWindow frees window and framebuffer inside
// deleteWindowMgr calls deleteWindow , fress self 
// deleteWindowNode calls deleteWindowMgr, frees self
// clearNodes runs deleteWindowNode on all nodes in screen buffer, frees
int deleteWindowMgr(w_windowManager_t *wm) {
  // window malloc'd. so free it the rest can just be freed normally
    if (wm == NULL) return -1;
    if (wm->aWindow != NULL) {
      // 
        deleteWindow(wm->aWindow);
        wm->aWindow = NULL;
    }
    free(wm);
    wm = NULL;
    return 0;
}
int deleteWindowNode(w_windowManagerNode_t *node) {
    if (node == NULL) return -1;
    // free the window inside the window manager
    if (node->windowMgr != NULL) { // gets here, all fine. aWindow=pointer to window
        deleteWindowMgr(node->windowMgr);
        node->windowMgr = NULL;
    }
    free(node);
    node = NULL;
    return 0;
}

int clearNodes(screenBuffer_t *screen) {
    if (screen == NULL) return -1;
    w_windowManagerNode_t *current = screen->head;
    while (current != NULL) {
        w_windowManagerNode_t *next = current->next;
        deleteWindowNode(current);
        // why are we leaking 56 bytes here????
        current = next;
    }
    screen->head = NULL;
    screen->numWindows = 0;
    return 0;
}

int clearDrawBuff(screenBuffer_t *screen){
    // clearnodes, set numwindows to 0
    if (screen == NULL) return -1;
    return clearNodes(screen);
}
int deleteScreen(screenBuffer_t *screen){
    if (screen == NULL) return -1;
    clearNodes(screen);
    free(screen);
    return 0;
}
// sort by zOrder ascending
int sortNodesByZOrder(screenBuffer_t *screen) {
    if (screen == NULL || screen->head == NULL) return -1;
    // Bubble sort for simplicity
    int swapped;
    do {
        swapped = 0;
        w_windowManagerNode_t *current = screen->head;
        while (current->next != NULL) {
            if (current->windowMgr->zOrder > current->next->windowMgr->zOrder) {
                // Swap windowMgrs
                w_windowManager_t *temp = current->windowMgr;
                current->windowMgr = current->next->windowMgr;
                current->next->windowMgr = temp;
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
    return 0;
}

screenBuffer_t *initWDrawTool(unsigned int termRows, unsigned int termCols) {
    screenBuffer_t *sb = malloc(sizeof(screenBuffer_t));
    sb->rows = termRows;
    sb->cols = termCols;
    sb->head = NULL;
    sb->numWindows = 0;
    return sb;
}

w_window_t *w_createWindow(unsigned int rows, unsigned int cols, unsigned int x, unsigned int y, const char* title, int active){
    w_window_t* win = malloc(sizeof(w_window_t));
    if (win == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for window\n");
        return NULL;
    }
    win->id = 0; // could be set to something meaningful later
    win->rows = rows;
    win->cols = cols;
    win->x = x;
    win->y = y;
    strncpy(win->title, title, 23);
    win->title[23] = '\0'; // ensure null termination
    win->active = active;

    // Allocate framebuffer
    win->fb.cells = (w_frame_t*)malloc(rows * cols * sizeof(w_frame_t));
    if (win->fb.cells == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for framebuffer\n");
        exit(1);
    }
    // Initialize framebuffer cells to empty
    for (unsigned int r = 0; r < rows; r++) {
        for (unsigned int c = 0; c < cols; c++) {
            w_frame_t *cell = &win->fb.cells[r * cols + c];
            cell->ch = 0; // empty
            cell->color = 0; // default color
            cell->flags = 0; // no flags
        }
    }
    return win;
}



int w_draw(screenBuffer_t *screen, const w_window_t *win){
  // queues drawing of window at 0,0,
  // gens a window manager for it and adds to screen buffer
  if (screen == NULL || win == NULL) return -1;
  w_windowManager_t *wm = malloc(sizeof(w_windowManager_t));
  if (wm == NULL) return -1;
  // make a copy of window, not reference
  wm->aWindow = malloc(sizeof(w_window_t));
  if (wm->aWindow == NULL) return -1;
  // memcpy(wm->aWindow, win, sizeof(w_window_t)); 
  // i really just wanna copy the framebuffer properly
  memcpy(wm->aWindow, win, sizeof(w_window_t));
  // deep copy framebuffer
  wm->aWindow->fb.cells = malloc(win->rows * win->cols * sizeof(w_frame_t));
  if (wm->aWindow->fb.cells == NULL) {
      free(wm->aWindow);
      free(wm);
      return -1;
  }
  memcpy(wm->aWindow->fb.cells, win->fb.cells, win->rows * win->cols * sizeof(w_frame_t));
  wm->x = 0;
  wm->y = 0;
  wm->zOrder = screen->numWindows; // draw in order added
  int res = addNewNode(screen, wm);
  if (res < 0) {
      free(wm->aWindow);
      free(wm);
      return res;
  }
  
 return 0;
}
int w_drawAt(screenBuffer_t *screen, const w_window_t *win, unsigned int x, unsigned int y){
  // queues drawing of window at x,y
  // gens a window manager for it and adds to screen buffer
  if (screen == NULL || win == NULL) return -1;
  w_windowManager_t  *wm = malloc(sizeof(w_windowManager_t));
  // make a copy of window, not reference
  wm->aWindow = malloc(sizeof(w_window_t));
  if (wm->aWindow == NULL) return -1;
  // memcpy(wm->aWindow, win, sizeof(w_window_t));
    // i really just wanna copy the framebuffer properly
  memcpy(wm->aWindow, win, sizeof(w_window_t));
  // deep copy framebuffer
  wm->aWindow->fb.cells = malloc(win->rows * win->cols * sizeof(w_frame_t));
  if (wm->aWindow->fb.cells == NULL) {
      free(wm->aWindow);
      free(wm);
      return -1;
  }
  memcpy(wm->aWindow->fb.cells, win->fb.cells, win->rows * win->cols * sizeof(w_frame_t));
  wm->x = x;
  wm->y = y;
  wm->zOrder = screen->numWindows; // draw in order added
  int res = addNewNode(screen, wm);
  if (res < 0) {
      free(wm->aWindow);
      return res;
  }
 return 0;
}


int putFrame(w_frame_t printMe) {
  // if color >0
    uint32_t ch = printMe.ch; 
    uint8_t frameColor = printMe.color;
    uint8_t frameFlags = printMe.flags;
    // if color flag set (bitwise b/c multiple flags possible)
    // print the color escape code
    if (frameFlags & WF_USE_COLOR) {
        printf("%s", COLOR_ARRAY[frameColor]);
    }
    if (ch <= 0x7F) {
        putchar((char)ch);
    } else if (ch <= 0x7FF) {
        putchar((char)(0xC0 | ((ch >> 6) & 0x1F)));
        putchar((char)(0x80 | (ch & 0x3F)));
    } else if (ch <= 0xFFFF) {
        putchar((char)(0xE0 | ((ch >> 12) & 0x0F)));
        putchar((char)(0x80 | ((ch >> 6) & 0x3F)));
        putchar((char)(0x80 | (ch & 0x3F)));
    } else {
        putchar((char)(0xF0 | ((ch >> 18) & 0x07)));
        putchar((char)(0x80 | ((ch >> 12) & 0x3F)));
        putchar((char)(0x80 | ((ch >> 6) & 0x3F)));
        putchar((char)(0x80 | (ch & 0x3F)));
    }
    // if it has flag for reset after, do that
    if (frameFlags & WF_RESET_AFTER) {
        printf("%s", A_RESET);
    }
    return 0;
}

int putRawChar(uint32_t ch, uint8_t color) {
    // conv to frame
    // color 0 none, greater, set color to color-1 and flags for color+reset
    w_frame_t frame;
    frame.ch = ch;
    if (color > 0) {
        frame.color = (color - 1);
        frame.flags = WF_USE_COLOR | WF_RESET_AFTER;
    } else {
        frame.color = 0;
        frame.flags = 0;
    }
    putFrame(frame);
    return 0;
}
int intern_w_draw(const w_window_t *win){  
  if (win == NULL) return -1;
  printf(A_ESC "2J"); // clear screen
  printf(A_ESC "H");  // reset 
  for (unsigned int r = 0; r < win->rows; r++) {
    for (unsigned int c = 0; c < win->cols; c++) {
      w_frame_t *cell = &win->fb.cells[r * win->cols + c];
      // must not just be handled like strings for. some reason

      // this looks insane but i walk thru it in  intern_w_drawAt 
      if (cell->ch == 0) {
        putchar(' '); // empty put nothing
      } else {
        if (cell->flags & WF_USE_COLOR) {
          printf("%s", COLOR_ARRAY[cell->color]);
        }
        if (cell->ch <= 0x7F) {
          putchar((char)cell->ch);
        } else if (cell->ch <= 0x7FF) {
          putchar((char)(0xC0 | ((cell->ch >> 6) & 0x1F)));
          putchar((char)(0x80 | (cell->ch & 0x3F)));
        } else if (cell->ch <= 0xFFFF) {
          putchar((char)(0xE0 | ((cell->ch >> 12) & 0x0F)));
          putchar((char)(0x80 | ((cell->ch >> 6) & 0x3F)));
          putchar((char)(0x80 | (cell->ch & 0x3F)));
        } else {
          putchar((char)(0xF0 | ((cell->ch >> 18) & 0x07)));
          putchar((char)(0x80 | ((cell->ch >> 12) & 0x3F)));
          putchar((char)(0x80 | ((cell->ch >> 6) & 0x3F)));
          putchar((char)(0x80 | (cell->ch & 0x3F)));
        }
        if (cell->flags & WF_RESET_AFTER) {
          printf("%s", A_RESET);
        }
      }
    }
    putchar('\n');
  } 
  return 0;
}
int intern_w_drawAt(const w_windowManager_t *wm) {
    if (wm == NULL || wm->aWindow == NULL) return -1;
    // save cursor to restore later
    printf(A_ESC "s");
    unsigned int x = wm->x;
    unsigned int y = wm->y;
    w_window_t *win = wm->aWindow;
    for (unsigned int r = 0; r < win->rows; r++) {
        printf(A_ESC "%u;%uH", (unsigned)(y + r + 1), (unsigned)(x + 1));
        for (unsigned int c = 0; c < win->cols; c++) {
            w_frame_t *cell = &win->fb.cells[r * win->cols + c];

            // drawAt used for partial updates, so must skip empty to draw alongside
            if (cell->ch == 0) {
                printf(A_ESC "1C");
                continue;
            }
            // if color flag set (bitwise b/c multiple flags possible)
            // print the color escape code
            if (cell->flags & WF_USE_COLOR) {
                printf("%s", COLOR_ARRAY[cell->color]);
            }

            // this was the only way i got the utf-8 to work properly ack
            // utf-8 encoding..
            // from wikipedia:
            // UTF-8 encodes code points in one to four bytes, depending on the value of the code point
            // first code point - last code point
            /// position of U+uvwxyz:
            // 0x0000–0x007F    -> 	0yyyzzzz
            // 0x0080–0x07FF    -> 110xxxyy 10yyzzzz
            // 0x0800–0xFFFF    -> 1110wwww 10xxxxyy 10yyzzzz
            // 0x10000–0x10FFFF	-> 11110uvv 10vvwwww 10xxxxyy 10yyzzzz
            // REALISTICALLY. we wont ever really hit beyond 2? bytes but its not much extra work to do it all ig
            // HENCE. 4 cases:

            // if <= 0x7F, its single byte (meaning ACII compatible, and can raw print as char!)
            // b/c 0x7F = 0111 1111 -> needs 7 bits 
            // while technically its the    0yyyzzzz and 3 + 4, works safely to just raw print as char        
            if (cell->ch <= 0x7F) {
                putchar((char)cell->ch);
            }
            /*
             * if <= 0x7FF, its 2 byte utf-8
             * b/c 0x7FF = 0000 0111 1111 1111 -> needs 11 bits
             * recall:
             * 110xxxyy 10yyzzzz
             * so first byte is 110 + top 5 bits
             * second byte is 10 + bottom 6 bits
             * the 2 masks:
             * 0xC0 -> 110|0 0000 
             * 0x80 -> 10|00 0000
             * bc its 2 bytes long, we put 2 characters on outout, which then gets processed into the utf-8 char by terminal
             * so. cell->ch >> 6 gets top 5 bits (mapping to the 110xxxyy part).
             * masking with 0x1F (0001 1111) means  we pull out the xxxyy, then combine with 0xC0 to get full first byte
             */
            else if (cell->ch <= 0x7FF) {
                putchar((char)(0xC0 | ((cell->ch >> 6) & 0x1F)));
                putchar((char)(0x80 | (cell->ch & 0x3F)));
            } 
            //!! EXAMPLE CASE: e wanna print"┼".  this maps to U+253C
            /* 
             * if <= 0xFFFF, its 3 byte utf-8
             * b/c 0xFFFF = 1111 1111 1111 1111 -> needs 11 bits
             * recall:
             * 1110wwww 10xxxxyy 10yyzzzz
             * so first byte is 1110 + top 4 bits
             * second byte is 10 + bottom 6 bits
             * third byte is 10 + bottom 6 bits
             * the 3 masks:
             * 0xE0 -> 1110| 0000 
             * 0x80 -> 10|00 0000
             * 0x80 -> 10|00 0000 
             * bc its 3 bytes long, we put 3 characters on outout, which then gets processed into the utf-8 char by terminal
             * so. example:
             * !! REMEMBER, Frame doesnt store a char, but a uint32_t. the value inside the cell we are looking at is
             *  0x253C = 0b 0010 0101 0011 1100  = 9532
             *  we pass first if bc 0x253C > 0x7F
             *  we pass second if bc 0x253C < 0x7FF
             *  then, because 0x253C < 0xFFFF, we enter this case:
             *  we currently have value of  BUT REMEMBER IT IS 32 BITS WIDE, so
             *  0x25 3C -> 0010 0101 0011 1100
             *  but truly: 0000 0000 0000 0000 0010 0101 0011 1100
             *  we shift right 12 to get top 4 bits, and mask with 0x0F to get just those 4 (0x0000 1111). then we add 0xE0 (1110 0000)
             *  resulting in the matched pattern of 1110wwww
             *  for second byte, we shift right 6 to get next 6 bits, mask with 0x3F (0011 1111) to get just those 6 bits, then add 0x80 (1000 0000)
             *  resulting in matched pattern of 10xxxxyy
             *  for third byte, we mask with 0x3F (0011 1111) to get bottom 6 bits, then add 0x80 (1000 0000)
             *  resulting in matched pattern of 10yyzzzz
             *  by writing all 3 bytes to output, terminal combines into single utf-8 char
             *  and we get a ┼ :3
             *  
             */
            
            else if (cell->ch <= 0xFFFF) {
                putchar((char)(0xE0 | ((cell->ch >> 12) & 0x0F)));
                putchar((char)(0x80 | ((cell->ch >> 6) & 0x3F)));
                putchar((char)(0x80 | (cell->ch & 0x3F)));
            } else {
                putchar((char)(0xF0 | ((cell->ch >> 18) & 0x07)));
                putchar((char)(0x80 | ((cell->ch >> 12) & 0x3F)));
                putchar((char)(0x80 | ((cell->ch >> 6) & 0x3F)));
                putchar((char)(0x80 | (cell->ch & 0x3F)));
            }
            // if it has flag for reset after, do that
            if (cell->flags & WF_RESET_AFTER) {
                printf("%s", A_RESET);
            }
        }
    }
    // restore cursor for sanity
    printf(A_ESC "u");
    fflush(stdout);
    return 0;
}
// if int is negative, rounded corners used
int w_boxAWindow(w_window_t *win, int mode){
    printf("Boxing window with mode %d\n", mode);

    int tl, tr, bl, br, h, v, t, b, l, r, tlr, blr;
    // this looks psychotic so heres the breakdown:
    // tl = top left corner: ┌
    // tr = top right corner: ┐
    // bl = bottom left corner: └
    // br = bottom right corner: ┘
    // h = horizontal line: ─
    // v = vertical line: │
    // t = tee top: ┬
    // b = tee bottom: ┴
    // l = tee left: ├
    // r = tee right: ┤
    // tlr = tee left right: ┬
    // blr = tee bottom left right: ┴
    // yk. might make  more sense to turn these into a const and index with a struct:
    // yea lets do that:
    // ENUM TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, HORIZONTAL, VERTICAL, TEE_TOP, TEE_BOTTOM, TEE_LEFT, TEE_RIGHT, TEE_LR, TEE_BOTTOM_LR
    // can i redo the lineType and cornerType to be created via the now defined enums?
    // example call: // w_boxAWindow(myWindow, DOUBLE | ROUNDED);
    // w_boxAWindow(myWindow, SINGLE);
    // fix these definitions  
    int lineType = mode & 0x3; // lower 2 bits
    if (lineType < 0 || lineType > 3) {
        return -1; // invalid line type
    }
    int cornerType = (mode & 0x4) >> 2; // 3rd bit
    if (cornerType < 0 || cornerType > 1) {
        return -1; // invalid corner type
    }
    // room for expansion later

    switch(lineType){
        case SINGLE: // single line
            h = LINEMODES[SINGLE][HORIZONTAL]; v = LINEMODES[SINGLE][VERTICAL];
            if(cornerType == 0){ // default corners
                tl = LINEMODES[SINGLE][TOPLEFT]; tr = LINEMODES[SINGLE][TOPRIGHT]; bl = LINEMODES[SINGLE][BOTTOMLEFT]; br = LINEMODES[SINGLE][BOTTOMRIGHT];
            } else { // rounded corners
                tl = ROUNDED_CORNERS[0]; tr = ROUNDED_CORNERS[1]; bl = ROUNDED_CORNERS[2]; br = ROUNDED_CORNERS[3];
            }
            t = LINEMODES[SINGLE][TEE_TOP]; b = LINEMODES[SINGLE][TEE_BOTTOM]; l = LINEMODES[SINGLE][TEE_LEFT]; r = LINEMODES[SINGLE][TEE_RIGHT]; tlr = LINEMODES[SINGLE][TEE_LR]; blr = LINEMODES[SINGLE][TEE_BOTTOM_LR];
            break;
        case HEAVY: // heavy line
            h = LINEMODES[HEAVY][HORIZONTAL]; v = LINEMODES[HEAVY][VERTICAL];
            if(cornerType == 0){ // default corners
                tl = LINEMODES[HEAVY][TOPLEFT]; tr = LINEMODES[HEAVY][TOPRIGHT]; bl = LINEMODES[HEAVY][BOTTOMLEFT]; br = LINEMODES[HEAVY][BOTTOMRIGHT];
            } else { // rounded corners
                tl = ROUNDED_CORNERS[0]; tr = ROUNDED_CORNERS[1]; bl = ROUNDED_CORNERS[2]; br = ROUNDED_CORNERS[3];
            }
            t = LINEMODES[HEAVY][TEE_TOP]; b = LINEMODES[HEAVY][TEE_BOTTOM]; l = LINEMODES[HEAVY][TEE_LEFT]; r = LINEMODES[HEAVY][TEE_RIGHT]; tlr = LINEMODES[HEAVY][TEE_LR]; blr = LINEMODES[HEAVY][TEE_BOTTOM_LR];
            break;
        case DOUBLE: // double line
            h = LINEMODES[DOUBLE][HORIZONTAL]; v = LINEMODES[DOUBLE][VERTICAL];
            if(cornerType == 0){ // default corners
                tl = LINEMODES[DOUBLE][TOPLEFT]; tr = LINEMODES[DOUBLE][TOPRIGHT]; bl = LINEMODES[DOUBLE][BOTTOMLEFT]; br = LINEMODES[DOUBLE][BOTTOMRIGHT];
            } else { // rounded corners
                tl = LINEMODES[DOUBLE][TOPLEFT]; tr = LINEMODES[DOUBLE][TOPRIGHT]; bl = LINEMODES[DOUBLE][BOTTOMLEFT]; br = LINEMODES[DOUBLE][BOTTOMRIGHT];
            }
            t = LINEMODES[DOUBLE][TEE_TOP]; b = LINEMODES[DOUBLE][TEE_BOTTOM]; l = LINEMODES[DOUBLE][TEE_LEFT]; r = LINEMODES[DOUBLE][TEE_RIGHT]; tlr = LINEMODES[DOUBLE][TEE_LR]; blr = LINEMODES[DOUBLE][TEE_BOTTOM_LR];
            break;
        case DASHED: // dashed line
            h = LINEMODES[DASHED][HORIZONTAL]; v = LINEMODES[DASHED][VERTICAL];
            if(cornerType == 0){ // default corners
                tl = LINEMODES[DASHED][TOPLEFT]; tr = LINEMODES[DASHED][TOPRIGHT]; bl = LINEMODES[DASHED][BOTTOMLEFT]; br = LINEMODES[DASHED][BOTTOMRIGHT];
            } else { // rounded corners
                tl = ROUNDED_CORNERS[0]; tr = ROUNDED_CORNERS[1]; bl = ROUNDED_CORNERS[2]; br = ROUNDED_CORNERS[3];
            }
            t = LINEMODES[DASHED][TEE_TOP]; b = LINEMODES[DASHED][TEE_BOTTOM]; l = LINEMODES[DASHED][TEE_LEFT]; r = LINEMODES[DASHED][TEE_RIGHT]; tlr = LINEMODES[DASHED][TEE_LR]; blr = LINEMODES[DASHED][TEE_BOTTOM_LR];
            break;
        default:
            return -1; // invalid mode
    }
    // draw top border
    win->fb.cells[0].ch = tl;
    for(unsigned int c = 1; c < win->cols - 1; c++){
        win->fb.cells[c].ch = h;
        // reset cell color/flags just in case
        win->fb.cells[c].color = 0;
        win->fb.cells[c].flags = 0;
    }
    win->fb.cells[win->cols - 1].ch = tr;
    // draw bottom border
    win->fb.cells[(win->rows - 1) * win->cols].ch = bl;
    for(unsigned int c = 1; c < win->cols - 1; c++){
        win->fb.cells[(win->rows - 1) * win->cols + c].ch = h;
        win->fb.cells[(win->rows - 1) * win->cols + c].color = 0;
        win->fb.cells[(win->rows - 1) * win->cols + c].flags = 0;
    }
    win->fb.cells[(win->rows - 1) * win->cols + win->cols - 1].ch = br;
    // draw left and right borders
    for(unsigned int r = 1; r < win->rows - 1; r++){
        win->fb.cells[r * win->cols].ch = v;
        win->fb.cells[r * win->cols + win->cols - 1].ch = v;
        win->fb.cells[r * win->cols].color = 0;
        win->fb.cells[r * win->cols].flags = 0;
        win->fb.cells[r * win->cols + win->cols - 1].color = 0;
        win->fb.cells[r * win->cols + win->cols - 1].flags = 0;
    }
    return 0;
}

int wRenderScreen(screenBuffer_t *screen){
    if (screen == NULL) return -1;
    // sort by zOrder first
    sortNodesByZOrder(screen);
    // draw each window in order
    w_windowManagerNode_t *current = screen->head;
    while (current != NULL) {
        intern_w_drawAt(current->windowMgr);
        current = current->next;
    }
    fflush(stdout);
    return 0;
}
int wSetTermClear(){
    printf(A_ESC "2J"); // clear screen
    printf(A_ESC "H");  // reset cursor
    fflush(stdout);
    return 0;
}
int fillWithChar(w_window_t *win, uint32_t ch){
  // convert frame into a char, then can direct memset it
  if (win == NULL) return -1;
  w_frame_t frame;
  frame.ch = ch;
  frame.color = 0;
  frame.flags = 0;
  return fillWithFrame(win, frame);
}
int fillWithFrame(w_window_t *win, const w_frame_t frame){
  if (win == NULL) return -1;
  for (unsigned int r = 0; r < win->rows; r++) {
    for (unsigned int c = 0; c < win->cols; c++) {
      w_frame_t *cell = &win->fb.cells[r * win->cols + c];
      cell->ch = frame.ch;
      cell->color = frame.color;
      cell->flags = frame.flags;
    }
  }
  return 0;
}
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
