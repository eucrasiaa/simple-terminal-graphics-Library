
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
devkit_init_t devkit_init() {
  int shm_id = shmget(DEBUGKEY, SHM_SIZE, IPC_CREAT | 0666);
  if (shm_id < 0) {
    perror("shmget failed");
    return (devkit_init_t){NULL, NULL, -1};
  }
  // attach
  void *shm_ptr = shmat(shm_id, NULL, 0);
  printf("Shared memory segment created with ID: %d\n", shm_id);
  printf("Shared memory attached at address: %p\n", shm_ptr);
  void *ptr = shm_ptr;
  // mutex is at start of shm
  pthread_mutex_t *mutex = (pthread_mutex_t *)ptr;
  return (devkit_init_t){shm_ptr, mutex, shm_id};
}
devkit_init_t watcher_init() {
  // same but doesnt create shm or init mutex, just grabs the existing ones
  int shm_id = shmget(DEBUGKEY, SHM_SIZE, 0666);
  if (shm_id < 0) {
    perror("shmget failed");
    return (devkit_init_t){NULL, NULL, -1};
  }
  // attach to the shared memory segment
  void *shm_ptr = shmat(shm_id, NULL, 0);
  if (shm_ptr == (void *)-1) {
    perror("shmat failed");
    return (devkit_init_t){NULL, NULL, -1};
  }
  void *ptr = shm_ptr;
  // mutex is at start of shm
  pthread_mutex_t *mutex = (pthread_mutex_t *)ptr;

  return (devkit_init_t){shm_ptr, mutex, shm_id};
}
int kill_devkit(devkit_init_t devkit_data) {
    if (shmdt(devkit_data.shm_ptr) < 0) {
      perror("shmdt failed");
      return -1;
    }
    // mark for deletion
    if (shmctl(devkit_data.shm_id, IPC_RMID, NULL) < 0) {
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
