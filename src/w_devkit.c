
// #include "w_devkit.h"
#include "../include/w_devkit.h"
/*

typedef struct {
    void *shm_ptr;
    pthread_mutex_t *mutex;
    int shm_id;
} devkit_init_t;
 */

int resetShareMem(){
  int shm_id = shmget(DEBUGKEY, SHM_SIZE, IPC_CREAT | 0666);
  void *shm_ptr = shmat(shm_id, NULL, 0);
  shmdt(shm_ptr);
  shmctl(shm_id, IPC_RMID, NULL);
  return 0;
}

shared_block_t* devkit_init() {
  int shm_id = shmget(DEBUGKEY, SHM_SIZE, IPC_CREAT | 0666);
  if (shm_id < 0) {
    perror("shmget failed");
    return NULL;
  }
  
  // attach
  void *shm_ptr = shmat(shm_id, NULL, 0);
  printf("Shared memory segment created with ID: %d\n", shm_id);
  printf("Shared memory attached at address: %p\n", shm_ptr);
  shared_block_t* sharedBlock = (shared_block_t *)shm_ptr;
  sharedBlock->header.shm_id = shm_id;
  return sharedBlock;
}
shared_block_t* watcher_init() {
  // should ONLY run after init has created shm
  // same but doesnt create shm or init mutex, just grabs the existing ones
  int shm_id = shmget(DEBUGKEY, SHM_SIZE, 0666);
  if (shm_id < 0) {
    perror("shmget failed");
    return NULL;
  }
  // attach to the shared memory segment
  void *shm_ptr = shmat(shm_id, NULL, 0);
  if (shm_ptr == (void *)-1) {
    perror("shmat failed");
    return NULL;
  }
  // the shm_id is already in the shared memory header so doesnt need to be set again
  return (shared_block_t *)shm_ptr;
}


void dvkt_init_critSyncStuff(shared_block_t *sb){
  pthread_mutexattr_t mattr;
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  
  pthread_mutex_init(&sb->header.mutex, &mattr);
  pthread_condattr_t cattr;
  pthread_condattr_init(&cattr);
  pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

  pthread_cond_init(&sb->header.cond, &cattr);
  printf("Devkit: Initialized critical sync stuff\n");
  // validate it all init'd correctly
  
}

void pre_write(shared_block_t* blk, shm_data_flags_t flag, int data_size){
  printf("Devkit: Preparing to write to shared memory\n");
  pthread_mutex_lock(&blk->header.mutex);
  blk->header.data_flags = flag;
  // blk->header.data_size = data_size;
  switch(flag){
    case SHM_FLAG_DEV_INT32:
      blk->header.data_size = sizeof(int32_t) * data_size;
      break;
    case SHM_FLAG_DEV_CHARBUF:
      blk->header.data_size = sizeof(char) * data_size;
      break;
    case SHM_FLAG_DEV_INT8:
      blk->header.data_size = sizeof(int8_t) * data_size;
      break;
    default:
      blk->header.data_size = data_size; // as is
      break;
  }
  printf("Devkit: Preparing to write to shared memory, should be writing now!\n");
}

void post_write(shared_block_t* blk){
  // signal cond and unlock mutex
  pthread_cond_signal(&blk->header.cond);
  pthread_mutex_unlock(&blk->header.mutex);
  printf("Devkit: Finished writing to shared memory, successfully ran post_write\n");
}
void cleanup_post_write(shared_block_t* blk){
  printf("Devkit: Cleaning up shared memory header after write\n");
  pthread_mutex_lock(&blk->header.mutex);
  blk->header.data_size = 0;
  blk->header.data_flags = SHM_FLAG_NONE;
  pthread_mutex_unlock(&blk->header.mutex);
  printf("Devkit: Cleaned up shared memory header after write\n");
}
// devkit_init_t devkit_init() {
//   int shm_id = shmget(DEBUGKEY, SHM_SIZE, IPC_CREAT | 0666);
//   if (shm_id < 0) {
//     perror("shmget failed");
//     return (devkit_init_t){NULL, NULL, -1};
//   }
//   // attach
//   void *shm_ptr = shmat(shm_id, NULL, 0);
//   printf("Shared memory segment created with ID: %d\n", shm_id);
//   printf("Shared memory attached at address: %p\n", shm_ptr);
//   void *ptr = shm_ptr;
//   // mutex is at start of shm
//   pthread_mutex_t *mutex = (pthread_mutex_t *)ptr;
//   return (devkit_init_t){shm_ptr, mutex, shm_id};
// }
// devkit_init_t watcher_init() {
//   // same but doesnt create shm or init mutex, just grabs the existing ones
//   int shm_id = shmget(DEBUGKEY, SHM_SIZE, 0666);
//   if (shm_id < 0) {
//     perror("shmget failed");
//     return (devkit_init_t){NULL, NULL, -1};
//   }
//   // attach to the shared memory segment
//   void *shm_ptr = shmat(shm_id, NULL, 0);
//   if (shm_ptr == (void *)-1) {
//     perror("shmat failed");
//     return (devkit_init_t){NULL, NULL, -1};
//   }
//   void *ptr = shm_ptr;
//   // mutex is at start of shm
//   pthread_mutex_t *mutex = (pthread_mutex_t *)ptr;

//   return (devkit_init_t){shm_ptr, mutex, shm_id};
// }
int kill_devkit( shared_block_t* sharedBlock) {
  printf("1\n");
    if (shmdt((void *)sharedBlock) < 0) {
      perror("shmdt failed");
      return -1;
    }
    printf("2\n");
    // ensure the header exists, and then also the id is valid
    if(sharedBlock == NULL) {
      fprintf(stderr, "kill_devkit: sharedBlock is NULL\n");
      return -1;
    }
    printf("3\n");
    if(sharedBlock->header.shm_id <= 0) {
      fprintf(stderr, "kill_devkit: invalid shm_id %d\n", sharedBlock->header.shm_id);
      return -1;
    }
    printf("4\n");
    if (shmctl(sharedBlock->header.shm_id, IPC_RMID, NULL) < 0) {
      perror("shmctl(IPC_RMID) failed");
      return -1; 
    }
    return 0;
}
int debug_connect() {
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
  if (connect(debug_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect failed");
    return -1;
  }
  return debug_fd;
}
// testing if it even works LOL with msgs
void debug_send(const char *msg) {
  if (debug_fd != -1) {
    // send(debug_fd, msg, strlen(msg), 0);
    // send vs write?
    write(debug_fd, msg, strlen(msg));
  }
}

void debug_close() {
  if (debug_fd != -1) {
    debug_send("<CMD><BREAK>");
    close(debug_fd);
    debug_fd = -1;
  }
}
