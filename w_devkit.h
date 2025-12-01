#ifndef W_DEVKILT_H
#define W_DEVKILT_H

#include <sys/socket.h>
#include <sys/un.h>


#define MY_SOCKET_PATH "/tmp/wtui_socket"


static int debug_fd = -1;

int debug_connect();
void debug_send(const char *msg);
void debug_close();



#endif // W_DEVKILT_H