
#include "w_devkit.h"
int debug_connect(){
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
    if (connect(debug_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect failed");
        return -1;
    }
    return debug_fd;
}
// testing if it even works LOL with msgs
void debug_send(const char *msg){
  if(debug_fd != -1){
    // send(debug_fd, msg, strlen(msg), 0);
    // send vs write?
    write(debug_fd, msg, strlen(msg));
  }
}

void debug_close(){
  if(debug_fd != -1){
    debug_send("<CMD><BREAK>");
    close(debug_fd);
    debug_fd = -1;
  }
}
