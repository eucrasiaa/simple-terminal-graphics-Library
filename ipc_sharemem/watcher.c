#include "library.h"
#include <sys/shm.h>
#include <unistd.h>



// this program will do a shared memory with main.c, main will share structs defined in library.h and this will read and print them out for testing
int main(){
    // how will this locate the shared memory created by main.c?
    // we can use shmget with the same key
    // but main.c uses IPC_PRIVATE, so we need to get the shm id from main. 
    int shm_id = shmget(DEBUGKEY, SHM_SIZE, 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        return 1;
    }
    // attach to the shared memory segment
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        return 1;
    }
    printf("Attached to shared memory at address: %p\n", shm_ptr);
    getchar(); // wait for user to be ready
    // busywait till data ready flag is set
    int *data_ready = (int *)shm_ptr;
    while (*data_ready != 1) {
        // not triggering, why?
        usleep(100000); // sleep 100ms
    }
    // now read the window struct
    void *ptr = shm_ptr + sizeof(int);
    w_window_t *shared_win = (w_window_t *)ptr;
    shared_win->fb.cells = (w_frame_t *)(ptr + sizeof(w_window_t));
    // print window info
    printW_info(shared_win);
    printf("Read window from shared memory:\n");
    return 0;
}