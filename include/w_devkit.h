#ifndef W_DEVKILT_H
#define W_DEVKILT_H

#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "w_types.h" 

#define MY_SOCKET_PATH "/tmp/wtui_socket"


// we gon do some shared memory stuff i think

static int debug_fd = -1;

int devkit_init();
int debug_connect();
void debug_send(const char *msg);
void debug_close();

#endif // W_DEVKILT_H