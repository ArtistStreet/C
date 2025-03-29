#include "lib.h"

#define port 8080
#define buffer_size 4096

const char *get_content_type(const char *path) {
    if (strstr(path, ".html")) {
        return "text/html";
    } else if (strstr(path, ".css")) {
        return "text/css";
    } else if (strstr(path, ".js")) {
        return "application/javascript";
    } 

    return "text/plain";
}

// server reads index file and send content to client using socket
void send_file_content(u_int32_t client_fd, const char *file_name) {
    FILE *file = fopen(file_name, "r");
    FILE *server_log = fopen("server.log", "a");

    if (file) {
        char response[buffer_size];
        char content[buffer_size];

        size_t byte_read = fread(content, 1, sizeof(content) - 1, file);
        content[byte_read] = '\0';
        fclose(file);

        // Create HTTP response
        snprintf(response, sizeof(response),
         "HTTP/1.1 200 OK\r\n"
         "Content-Type: %s\r\n"
         "Content-Length: %lu\r\n"
         "\r\n%s",
         get_content_type(file_name), (unsigned long)byte_read, content);

        // Send
        send(client_fd, response, strlen(response), 0);
    } else {
        char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, not_found, strlen(not_found), 0);
        if (server_log) {
            fwrite(not_found, 1, strlen(not_found), server_log);
        }
    }

    if (server_log) {
        fclose(server_log);
    }
}

void log_request(const char *client_ip, const char *method, const char *path, int status) {
    FILE *log_file = fopen("server.log", "a");
    if (log_file) {
        fprintf(log_file, "[%ld] Client IP: %s, Method: %s, Path: %s, Status: %d\n",
                time(NULL), client_ip, method, path, status);
        fclose(log_file);
    } else {
        return;
    }
}

void handle_client(u_int32_t client_fd) {
    FILE *server_log = fopen("server.log", "a");

    char buffer[buffer_size];
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read Failed");
        return;
    }

    buffer[bytes_read] = '\0';
    if (server_log) {
        // fwrite(buffer, 1, strlen(buffer), server_log);
    }

    if (strncmp(buffer, "GET ", 4) == 0) {
        char *start = buffer + 4; // start from 5th char
        char *end = strchr(start, ' ');
        *end = '\0';
        
        if (strcmp(start, "/") == 0) {
            send_file_content(client_fd, "Web/index.html");
        } else {
            char file_name[buffer_size];
            snprintf(file_name, sizeof(file_name), "Web%s", start);
            send_file_content(client_fd, file_name);
        }
    } else if (strncmp(buffer, "POST ", 5) == 0) {
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4;
            int8_t cnt = 0;
            char body_copy[100];

            strcpy(body_copy, body);
            char *token = strtok(body_copy, "&");
            char *name = NULL, *pass = NULL;
            while (token) {
                char *value = strchr(token, '=');
                if (value) { 
                    *value = '\0'; 
                    value++; 
                    if (cnt == 0) {
                        name = value;
                    } else if (cnt == 1) {
                        pass = value;
                    }
                    cnt++;
                }
                token = strtok(NULL, "&"); 
            }

            if (name && pass) {
                handle_token(name, pass); // Call the function from user_handle.c
            }
        }

        char response[buffer_size];
        snprintf(response, sizeof(response),
         "HTTP/1.1 200 OK\r\n"
         "Content-Type: text/plain\r\n"
         "Content-Length: 2\r\n"
         "\r\nOK");
        write(client_fd, response, strlen(response)); // Fix: send full response
    }

    if (server_log) {
        fclose(server_log);
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <html_file> <css_file> <js_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    printf("Server running on http://127.0.0.1:%d\n", port);

    while(1) {
        // accept connect from client
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
        close(client_fd);
    }

    close(server_fd);
}