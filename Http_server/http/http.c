#include "lib.h"

void http() {
    uint32_t server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr; // server address, client address
    socklen_t client_len = sizeof(client_addr); 
    
    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // ipv4, tcp
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // reuse socket
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // create server
    server_addr.sin_family = AF_INET; // ip for server
    server_addr.sin_addr.s_addr = INADDR_ANY; // listen from any ip
    server_addr.sin_port = htons(port); // run in 8080
    
    // bind socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // listen
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
}