#include "../lib.h"

char *get_client_ip(int client_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char *client_ip = (char *)malloc(INET_ADDRSTRLEN);

    if (client_ip && getpeername(client_fd, (struct sockaddr *)&addr, &addr_len) == 0) {
        inet_ntop(AF_INET, &addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        return client_ip;
    } else {
        perror("getpeername");
        free(client_ip);
        return NULL;
    }
}

void log_request(int client_fd, const char *method, const char *path) {
    // printf("debug: %s\n", path);
    FILE *log_file = fopen("log/server.log", "a");

    char *ip = get_client_ip(client_fd);
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    if (log_file) {
        fprintf(log_file, "%s - %s - %s - %s\n", time_str, ip, method, path);
        fclose(log_file);
    } else {
        return;
    }
}