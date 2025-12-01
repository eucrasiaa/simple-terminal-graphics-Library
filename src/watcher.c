// #include "w_drawingTool.h"
// #include "w_devkit.h"
#include "../include/w_devkit.h"
#include "../include/w_types.h"

// why does this 

#define BREAK_CMD 1000
int handleCommand(char* command, int length){
    printf("command caught: %.*s\n", length, command);
    if(length >= 7 && strncmp(command, "<BREAK>", 7) == 0){
        printf("<BREAK> called, exiting...\n");
        return BREAK_CMD;
    }
    return 0;
}
int main() {
    // void *win1, *win2, *screen;
    // read(STDIN_FILENO, &win1, sizeof(win1));
    // read(STDIN_FILENO, &win2, sizeof(win2));
    // read(STDIN_FILENO, &screen, sizeof(screen));
    // // printf("Received addresses: win1=%p, win2=%p, screen=%p\n", win1, win2, screen);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); 
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, MY_SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(MY_SOCKET_PATH); //  this removes any existing socket file, good practice?
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5); // arbetrary 5 ig
    printf("Debugger listening on %s\n", MY_SOCKET_PATH);
    // accept 
    int client_fd = accept(server_fd, NULL, NULL);
    char buffer[256];
    int listening = 1;
    while(listening){
        ssize_t numRead = read(client_fd, buffer, sizeof(buffer) - 1);
        
        if(numRead >= 5 && (strncmp(buffer, "<CMD>", 5) == 0)){
            printf("Command received: %.*s\n", (int)(numRead - 5), buffer + 5);
            int cmdout = handleCommand(buffer + 5, (int)(numRead - 5));
            if (cmdout == BREAK_CMD){
                listening = 0;
            }
            // command detected
            // char* endCmd = strstr(buffer, "<ENDCMD>");
            // if(endCmd != NULL){
            //     size_t cmdLength = endCmd - (buffer + 5); // length between <CMD> and <ENDCMD>
            //     int cmdResult = handleCommand(buffer + 5, cmdLength);
            //     if(cmdResult == BREAK_CMD){
            //         listening = 0;
            //     }
            // }
        }
        else if(numRead > 0){
            buffer[numRead] = '\0'; // null terminate, is this needed? ig just if its string its safe practice
            printf("%d: %s\n", server_fd, buffer);
        }
    }

    // close
    close(client_fd);
    close(server_fd);
    unlink(MY_SOCKET_PATH);

    // getchar(); // wait for input before exiting
    return 0;
}