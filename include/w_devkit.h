#ifndef W_DEVKILT_H
#define W_DEVKILT_H

#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
#include "w_types.h" 

#define DEBUGKEY 0xFEAB7389 // its literally nonsense i told diff friedns to gigve me numbers theres no deeper meaning
#define SHM_SIZE 0x100000 // 1 MB, seems reasonable for now?
#define MY_SOCKET_PATH "/tmp/wtui_socket"

// define flags marking the type of data w/i shared memory
// mem structure:
/*
 * memory layout:
 * [0-0x100000] (0-1048576), 1 MB of shared space
 * a frame 
 * 
 */
// const enum {}

// we gon do some shared memory stuff i think

// return struct for init that gives shared mem pointer, mutex, and socket fd

/*int shm_id = shmget(DEBUGKEY, SHM_SIZE, IPC_CREAT | 0666);
      if (shm_id < 0) {
          perror("shmget failed");
          return 1;
      }
      // attach
      void *shm_ptr = shmat(shm_id, NULL, 0);
      printf("Shared memory segment created with ID: %d\n", shm_id);
      printf("Shared memory attached at address: %p\n", shm_ptr);
      void *ptr = shm_ptr;
      // mutex init at start of shm
      pthread_mutex_t *mutex = (pthread_mutex_t *)ptr;
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
      if (pthread_mutex_init(mutex, &attr) != 0) {
          perror("Mutex initialization failed");
          return 1;
      }
          */
typedef struct {
    void *shm_ptr;
    pthread_mutex_t *mutex;
    int shm_id;
} devkit_init_t;
static int debug_fd = -1;

devkit_init_t devkit_init();
devkit_init_t watcher_init();
int kill_devkit();
int debug_connect();
void debug_send(const char *msg);
void debug_close();

#endif // W_DEVKILT_H