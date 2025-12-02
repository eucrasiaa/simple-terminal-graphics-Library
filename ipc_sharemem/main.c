#include "library.h"
#include <sys/shm.h>

/*
proper def of what the shared memory segment layout should do this
*/


int main(){
    // do a run of each function to test
    w_window_t *win = makeWin(5, 5);
    w_windowManager_t *wm = makeWinMgr(win, 1);
    screenBuffer_t *sb = makeSB_empty(24, 80);
    screenBuffer_t *sb_rand = makeSB_randomNode(24, 80);
    screenBuffer_t *sb_two = makeSB_twoNode(24, 80);
    printW_info(win);
    printWM_info(wm);
    printSB_info(sb);
    printSB_info(sb_rand);
    printSB_info(sb_two);
    // will write the structs to shared memory, watcher.c will be reading them
     // create shared memory segment
     // these flags: 
     //fixed key
    
    int shm_id = shmget(DEBUGKEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        return 1;
    }
    // attach
    void *shm_ptr = shmat(shm_id, NULL, 0);
    printf("Shared memory segment created with ID: %d\n", shm_id);
    printf("Shared memory attached at address: %p\n", shm_ptr);
    // i want my malloc'd pointers to be in shared memory, and a seperate program will read the shm. how is this done? 
    // you should allocate memory within the shared memory segment for your data structures instead of using malloc.
    // in shared memory, we will place a `1` at [0] to indicate data is ready, window at [1], framebuffer at [1+sizeof(window)]
    // so. lets open first with shm_open
    void *ptr = shm_ptr;;
    // write data ready flag
    getchar();
  
    ptr += sizeof(int);
    // write window data
    w_window_t *shared_win = (w_window_t *)ptr;
    memcpy(shared_win, win, sizeof(w_window_t));
    ptr += sizeof(w_window_t);
    // write window framebuffer data
    size_t fb_size = win->rows * win->cols * sizeof(w_frame_t);
    w_frame_t *shared_fb = (w_frame_t *)ptr;
    memcpy(shared_fb, win->fb.cells, fb_size);
    // Now the shared memory contains the window and its framebuffer.
    // now put at the start that data is ready
    // ptr = shm_ptr;
    int *data_ready = (int *)shm_ptr;
    *data_ready = 1; // indicate data is ready
    printf("Data written to shared memory. You can run watcher.c to read it.\n");
    // wait for user input before exiting
    getchar();
    // detach
    if (shmdt(shm_ptr) < 0) {
        perror("shmdt failed");
        return 1;
    }
    if (shmctl(shm_id, IPC_RMID, NULL) < 0) {
        perror("shmctl(IPC_RMID) failed");
        return 1; 
    }
    return 0;
}