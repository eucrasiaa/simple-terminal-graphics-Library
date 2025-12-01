#include "w_drawingTool.h"
#include <stdio.h>
#include <unistd.h>
int main() {
    void *win1, *win2, *screen;
    read(STDIN_FILENO, &win1, sizeof(win1));
    read(STDIN_FILENO, &win2, sizeof(win2));
    read(STDIN_FILENO, &screen, sizeof(screen));
    printf("Received addresses: win1=%p, win2=%p, screen=%p\n", win1, win2, screen);
    getchar(); // wait for input before exiting
    return 0;
}