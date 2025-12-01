
  #include "w_drawingTool.h"
  #include <stdlib.h>

/*
 * another tester
 * made by Will.
 * we ALL love ANSI escape codes its such a good system yes yes mhm (nods of agreement from the crowd) (cheers errupt. the whole audience is jumping and screaming and crying)
 * 11/29/2025 for project-5
 */


int TERM_WIDTH = 80;
int TERM_HEIGHT = 24;


int main(int argc, char *argv[]) {
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

  
  // make a window



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

  // can we setup a 1 way pipe to a second program which will watch the variables as we go through the program?
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    return 1;
  }
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  // give the addresses of the 2 windows + screenbuffer to the child process
  if (pid == 0) { // child process
    close(fd[1]); // close write end
    dup2(fd[0], STDIN_FILENO); // redirect stdin to read end of pipe
    execlp("./wwatch", "./wwatch", NULL); // replace with watcher program
    perror("execlp");
    exit(EXIT_FAILURE);
  } else { // parent process
    close(fd[0]); // close read end
    // write addresses to pipe
    write(fd[1], &myWin, sizeof(myWin));
    write(fd[1], &myWinSolid, sizeof(myWinSolid));
    write(fd[1], &myScreen, sizeof(myScreen));
    close(fd[1]); // close write end
  }

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
  wSetTermClear();
  wRenderScreen(myScreen);
  deleteScreen(myScreen);
  deleteWindow(myWin); //screen carries copy, so delete both
  deleteWindow(myWinSolid);
  getchar(); // pause to see
  return 0;
}