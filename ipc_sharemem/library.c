#include "library.h"

w_window_t *makeWin(unsigned int rows, unsigned int cols) {
  w_window_t *win = (w_window_t *)malloc(sizeof(w_window_t));
  if (win == NULL) {
    return NULL; // allocation failed
  }
  win->id = rand() % 1000 + 1; // just a test id, from 1 to 1000
  win->rows = rows;
  win->cols = cols;
  win->x = rand() % 10; // random position 0-9
  win->y = rand() % 10; // random position 0-9
  snprintf(win->title, sizeof(win->title), "Window %d", win->id);
  win->active = 1; // active by default
  // allocate framebuffer
  win->fb.cells = (w_frame_t *)malloc(rows * cols * sizeof(w_frame_t));
  if (win->fb.cells == NULL) {
    free(win);
    return NULL; // allocation failed
  }
  // initialize framebuffer cells to empty
  for (unsigned int i = 0; i < rows * cols; i++) {
    win->fb.cells[i].ch = 0;
    win->fb.cells[i].color = 0;
    win->fb.cells[i].flags = 0;
  }
  return win;
}
w_windowManager_t *makeWinMgr(w_window_t *win, unsigned int zOrder) {
  w_windowManager_t *wm = (w_windowManager_t *)malloc(sizeof(w_windowManager_t));
  if (wm == NULL) {
    return NULL; // allocation failed
  }
  wm->aWindow = win;
  wm->x = rand() % 21 + 10; // from 10 to 30
  wm->y = rand() % 21 + 10; // from 10 to 30
  wm->zOrder = zOrder;
  return wm;
}

screenBuffer_t *makeSB_empty(unsigned int termRows, unsigned int termCols) {
  screenBuffer_t *sb = (screenBuffer_t *)malloc(sizeof(screenBuffer_t));
  if (sb == NULL) {
    return NULL; // allocation failed
  }
  sb->rows = termRows;
  sb->cols = termCols;
  sb->head = NULL;
  sb->numWindows = 0;
  return sb;
}
int addNewNode(screenBuffer_t *screen, w_windowManager_t *wm) {
  // passed pointer is already malloc'd
  if (screen == NULL || wm == NULL)
    return -1;
  w_windowManagerNode_t *newNode = malloc(sizeof(w_windowManagerNode_t));
  if (newNode == NULL)
    return -1;
  // inserts at head,
  newNode->windowMgr = wm;
  newNode->next = screen->head;
  screen->head = newNode;
  screen->numWindows++;
  return 0;
}
screenBuffer_t *makeSB_randomNode(unsigned int termRows, unsigned int termCols) {
  // makes empty first
  screenBuffer_t *sb = makeSB_empty(termRows, termCols);
  if (sb == NULL) {
    return NULL; // allocation failed
  }
  w_window_t *win = makeWin(rand() % 6 + 5, rand() % 6 + 5); // from 5-10, 5-10
  if (win == NULL) {
    free(sb);
    return NULL; // allocation failed
  }
  w_windowManager_t *wm = makeWinMgr(win, 0);
  if (wm == NULL) {
    // dont free let it leajk
    return NULL; // allocation failed
  }
  addNewNode(sb, wm);
  return sb;
}
screenBuffer_t *makeSB_twoNode(unsigned int termRows, unsigned int termCols) {
  // makes empty first
  screenBuffer_t *sb = makeSB_empty(termRows, termCols);
  if (sb == NULL) {
    return NULL; // allocation failed
  }
  w_window_t *win1 = makeWin(rand() % 6 + 5, rand() % 6 + 5); // from 5-10, 5-10
  if (win1 == NULL) {
    free(sb);
    return NULL; // allocation failed
  }
  w_windowManager_t *wm1 = makeWinMgr(win1, 1);
  if (wm1 == NULL) {
    // dont free let it leajk
    return NULL; // allocation failed
  }
  addNewNode(sb, wm1);
  w_window_t *win2 = makeWin(rand() % 6 + 5, rand() % 6 + 5); // from 5-10, 5-10
  if (win2 == NULL) {
    free(sb);
    return NULL; // allocation failed
  }
  w_windowManager_t *wm2 = makeWinMgr(win2, 0);
  if (wm2 == NULL) {
    // dont free let it leajk
    return NULL; // allocation failed
  }
  addNewNode(sb, wm2);
  return sb;
}

void printW_info(w_window_t *win) {
  // basic print, assume frames are just numbers and not full utf8
  if (win == NULL) {
    printf("Window is NULL\n");
    return;
  }
  printf("Window ID: %d\n", win->id);
  printf("Title: %s\n", win->title);
  printf("Size: %ux%u\n", win->cols, win->rows);
  printf("Position: (%u, %u)\n", win->x, win->y);
  printf("Active: %d\n", win->active);
  printf("Framebuffer Cells:\n");
  for (unsigned int r = 0; r < win->rows; r++) {
    for (unsigned int c = 0; c < win->cols; c++) {
      w_frame_t *cell = &win->fb.cells[r * win->cols + c];
      printf("(%u,%u):[%u,%u,%u] | ", r, c, cell->ch, cell->color, cell->flags);
    }
    printf("\n");
  }
}
void printWM_info(w_windowManager_t *wm) {
  if (wm == NULL) {
    printf("Window Manager is NULL\n");
    return;
  }
  printf("Window Manager Position: (%u, %u)\n", wm->x, wm->y);
  printf("Z-Order: %u\n", wm->zOrder);
  printf("Associated Window Info:\n");
  printW_info(wm->aWindow);
}

void printSB_info(screenBuffer_t *sb) {
  if (sb == NULL) {
    printf("Screen Buffer is NULL\n");
    return;
  }
  printf("Screen Buffer Size: %ux%u\n", sb->cols, sb->rows);
  printf("Number of Windows: %u\n", sb->numWindows);
  w_windowManagerNode_t *current = sb->head;
  unsigned int index = 0;
  while (current != NULL) {
    printf("Window Manager Node %u Info:\n", index);
    printWM_info(current->windowMgr);
    current = current->next;
    index++;
  }
}
