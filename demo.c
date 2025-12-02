
#include "include/w_drawingTool.h"
#ifdef ENABLE_DEVKIT
#include "include/w_devkit.h"
#endif
/*
 * another tester
 * made by Will.
 * we ALL love ANSI escape codes its such a good system yes yes mhm (nods of agreement from the crowd) (cheers errupt. the whole audience is jumping and screaming and crying)
 * 11/29/2025 for project-5
 */


int TERM_WIDTH = 80;
int TERM_HEIGHT = 24;



int main(int argc, char *argv[]) {
  
  #ifdef ENABLE_DEVKIT
    resetShareMem();
    shared_block_t* w_shmblk = devkit_init();         // start unix socket + shared memory
    if (w_shmblk == NULL) {
      fprintf(stderr, "Failed to initialize devkit\n");
      return -1;
    }
    dvkt_init_critSyncStuff(w_shmblk);
    printf("id is %d\n", w_shmblk->header.shm_id);
    printf("DK Status: Shared Mem @ %p, Mutex @ %p\n", w_shmblk, &w_shmblk->header.mutex);
    // debug_connect();
    debug_send("Initialized drawing tool\n");
    void *shm_ptr = w_shmblk;
    printf("press enter to init dev write");
    // getchar();
    // writing 1 int32 to shared memory to test
    pre_write(w_shmblk, SHM_FLAG_DEV_INT32, 1);
    // now safely write at payload slot
    int32_t *shared_int = (int32_t *)(shm_ptr + SHM_HEADER_SIZE);
    *shared_int = 413;
    printf("Wrote int32 %d to shared memory\n", *shared_int);
    post_write(w_shmblk);
    printf("watcher SHOULD catch now.\npress enter to continue\n");
    // getchar();
    // // at end of intitialization, shared mem is attached and mutex is initialized and claimed
  #endif
  // sizeof all w_types
  printf("sizeof(screenBuffer_t) = %lu bytes\n", sizeof(screenBuffer_t));
  printf("sizeof(w_window_t) = %lu bytes\n", sizeof(w_window_t));
  printf("sizeof(w_windowManager_t) = %lu bytes\n", sizeof(w_windowManager_t));
  printf("sizeof(w_framebuffer_t) = %lu bytes\n", sizeof(w_framebuffer_t));
  printf("sizeof(w_frame_t) = %lu bytes\n", sizeof(w_frame_t));


  
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  TERM_WIDTH = w.ws_col;
  TERM_HEIGHT = w.ws_row;
  printf("Terminal size: %d cols, %d rows\n", TERM_WIDTH, TERM_HEIGHT);
  
  //initialize drawing tool
  screenBuffer_t *myScreen = initWDrawTool(TERM_HEIGHT, TERM_WIDTH);
  if (myScreen == NULL) {
    printf("ERROR: failed to initialize drawing tool\n");
    return -1;
  }



  w_window_t* myWin = w_createWindow(10, 40, 5, 5, "Test Window", 1);
  if (myWin == NULL) {
    printf("ERROR: failed to allocate memory for window\n");
    return -1;
  }

  // draw the window
  // pput some stuff in it
  const char *testStr = "abcde";
  for (size_t i = 0; i < strlen(testStr); i++) {
    myWin->fb.cells[i+62].ch = (uint32_t)testStr[i];
    myWin->fb.cells[i+62].color = 2; // green
    myWin->fb.cells[i+62].flags = WF_USE_COLOR | WF_RESET_AFTER;
  }
  w_window_t* myWinSolid = w_createWindow(10, 40, 5, 5, "Test Window", 1);
  for (unsigned int r = 0; r < myWinSolid->rows; r++) {
    for (unsigned int c = 0; c < myWinSolid->cols; c++) {
      w_frame_t *cell = &myWinSolid->fb.cells[r * myWinSolid->cols + c];
      cell->ch = (uint32_t)' ';
      cell->color = 0; // black
      cell->flags = 0;
    }
  }
  // printf("launch debugger!\n");
  // getchar();
  // debug_connect();
  // debug_send("test test test\n");

  // debug_close();
  
  // //print each style of box to verify correct values
  // for (int i = 0; i < 4; i++) { // Line type
  //   printf("Line type %d:\n", i);
  //   for (int j = 0; j <NUM_CHARS; j++) { // each char in set
  //     putRawChar(LINEMODES[i][j], 0); // single line
  //     printf(" ");
  //   }
  //   printf("\n");
  // }
  

  w_boxAWindow(myWin, SINGLE); // single line, sharp corners
  w_boxAWindow(myWinSolid, SINGLE|ROUNDED); // single line, sharp corners
  w_draw(myScreen, myWin);
  w_drawAt(myScreen, myWinSolid, 5, 5); // draw at 5,5
    getchar();
    initRawMode();
    getchar();
  wSetTermClear();

  wRenderScreen(myScreen);
  clearDrawBuff(myScreen);

  // getchar(); // pause to see
  // wSetTermClear();
  // fillWithChar(myWinSolid, 'X');
  // w_drawAt(myScreen, myWinSolid, 0, 0); // draw at 0,0
  // wRenderScreen(myScreen);
  // getchar(); // pause to see
  //

  deleteScreen(myScreen);
  deleteWindow(myWin); //screen carries copy, so delete both
  deleteWindow(myWinSolid);
  getchar(); // pause to see
  restoreTermMode();
  #ifdef ENABLE_DEVKIT
    debug_close();
    // kill_devkit(w_shmblk);
    resetShareMem();

    
  #endif
  return 0;
}