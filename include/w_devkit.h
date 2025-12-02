#ifndef W_DEVKILT_H
#define W_DEVKILT_H

#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "w_types.h" 

#define DEBUGKEY 0xFEAB7389 // its literally nonsense i told diff friedns to gigve me numbers theres no deeper meaning
#define MY_SOCKET_PATH "/tmp/wtui_socket"


// we gon do some shared memory stuff i think

static int debug_fd = -1;

int devkit_init();
int debug_connect();
void debug_send(const char *msg);
void debug_close();

#endif // W_DEVKILT_H