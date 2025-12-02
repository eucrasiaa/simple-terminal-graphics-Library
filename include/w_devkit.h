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

typedef enum {
    SHM_FLAG_NONE         = 0x0,
    SHM_FLAG_WINDOW       = 0x1,
    SHM_FLAG_WINDOWMGR    = 0x2,
    SHM_FLAG_FRAMEBUFFER  = 0x4,
    SHM_FLAG_SCREENBUFFER = 0x8,
    SHM_FLAG_DEV_INT32    = 0x10,
    SHM_FLAG_DEV_CHARBUF  = 0x20,
    SHM_FLAG_DEV_INT8     = 0x40,

} shm_data_flags_t;

#define SHM_HEADER_SIZE  (sizeof(pthread_mutex_t) + sizeof(pthread_cond_t) + sizeof(int32_t) + sizeof(shm_data_flags_t) + sizeof(int32_t))
#define SHM_PAYLOAD_SIZE (SHM_SIZE - sizeof(shared_block_t_header_t))

typedef struct {
    pthread_mutex_t   mutex;
    pthread_cond_t    cond;
    int32_t           shm_id;
    shm_data_flags_t  data_flags;
    int32_t           data_size;   // size in bytes
} shared_block_t_header_t;
typedef struct {
    shared_block_t_header_t header;
    unsigned char payload[];   // remainder of the 1MB region
} shared_block_t;

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
    int shm_id;
    shared_block_t *shared_block;
} devkit_init_t;
static int debug_fd = -1;

shared_block_t* devkit_init();
shared_block_t* watcher_init();

void dvkt_init_critSyncStuff(shared_block_t *sb);
int kill_devkit(shared_block_t* sharedBlock);
int debug_connect();
void debug_send(const char *msg);
void debug_close();
// sets locks for writing to shared memory
void pre_write(shared_block_t* blk, shm_data_flags_t flag, int data_size);
//writing done here in the program
void post_write(shared_block_t* blk); // send cond signal and unlock mutex
void cleanup_post_write(shared_block_t* blk); // zeros out data size and flags after write complete
#endif // W_DEVKILT_H