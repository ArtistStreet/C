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
    } else if (strstr(path, ".jpg")) {
        return "image/jpeg";
    } else if (strstr(path, ".png")) {
        return "image/png";
    }

    return "text/plain";
}

long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return 0;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

// server reads index file and send content to client using socket
void send_file_content(int client_fd, const char *file_name) {
    FILE *file;
    if (strstr(file_name, ".jpg") || strstr(file_name, ".png")) {
        file = fopen(file_name, "rb"); 
    } else {
        file = fopen(file_name, "r");
    }

<<<<<<< HEAD
    FILE *server_log = fopen("server.log", "a");
=======
    FILE *server_log = fopen("log/server.log", "a");
>>>>>>> 2f0e4a7 (update)

    if (file) {
        char header[256];
        snprintf(header, sizeof(header),
<<<<<<< HEAD
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: %s\r\n"
                 "Content-Length: %lu\r\n"
                 "\r\n",
                 get_content_type(file_name),
                 (unsigned long) get_file_size(file_name));
=======
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: %s\r\n"      
                "Content-Length: %lu\r\n"
                "\r\n",
                get_content_type(file_name),
                (unsigned long) get_file_size(file_name));
>>>>>>> 2f0e4a7 (update)

        send(client_fd, header, strlen(header), 0);

        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(client_fd, buffer, bytes_read, 0);
        }

        fclose(file);
    } else {
        char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, not_found, strlen(not_found), 0);
        if (server_log) {
            fprintf(server_log, "Error: Failed to open file %s\n", file_name);
            fwrite(not_found, 1, strlen(not_found), server_log);
        }
    }

    if (server_log) {
        fclose(server_log);
    }
}


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
<<<<<<< HEAD
    FILE *log_file = fopen("server.log", "a");
=======
    FILE *log_file = fopen("log/server.log", "a");
>>>>>>> 2f0e4a7 (update)

    char *ip = get_client_ip(client_fd);
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    if (log_file) {
<<<<<<< HEAD
        fprintf(log_file, "[%s] Client IP: %s Method: %s, Path: %s\n",
                time_str, ip, method, path);
=======
        // fprintf(log_file, "[%s] Client IP: %s Method: %s, Path: %s\n",
        //         time_str, ip, method, path);
>>>>>>> 2f0e4a7 (update)
        fclose(log_file);
    } else {
        return;
    }
}

<<<<<<< HEAD
void handle_client(u_int32_t client_fd, const char *html, const char *css, const char *js) {
    FILE *server_log = fopen("server.log", "a");
=======
void handle_client(u_int32_t client_fd, SSL_CTX *ctx, const char *html, const char *css, const char *js) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    if (SSL_accept(sl) <= 0) {

    }
    FILE *server_log = fopen("log/server.log", "a");
>>>>>>> 2f0e4a7 (update)

    char buffer[buffer_size];
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read Failed");
        return;
    }

    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "GET ", 4) == 0) {
        
        char *start = buffer + 4; // start from 5th char
        char *end = strchr(start, ' ');
        *end = '\0';
        // printf("debug: %s\n", start);
        fflush(stdout);
        
        if (strcmp(start, "/") == 0) {
            log_request(client_fd, "GET", html);
            send_file_content(client_fd, html);
        } else {
            if (strstr(start, ".css")) {
                log_request(client_fd, "GET", css);
                send_file_content(client_fd, css);
            } else if (strstr(start, ".js")) {
                log_request(client_fd, "GET", js);
                send_file_content(client_fd, js);
            } else if (strstr(start, ".jpg") || strstr(start, ".png")) {
                char image_path[256];
                snprintf(image_path, sizeof(image_path), "Web%s", start); // Thêm "web/"
                // printf("%s", image_path);
                log_request(client_fd, "GET", image_path);
                send_file_content(client_fd, image_path);
                // log_request(client_fd, "GET", start + 1); // Remove leading '/'
                // send_file_content(client_fd, start + 1);
            } else {
                char not_found[] = "HTTP/1.1 404 Not Found\r\n\r\n";
                send(client_fd, not_found, strlen(not_found), 0);
            }
        }
    } else if (strncmp(buffer, "POST ", 5) == 0) {
        log_request(client_fd, "POST", css);
        
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

<<<<<<< HEAD
    uint32_t server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr; // server address, client address
    socklen_t client_len = sizeof(client_addr); 

    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // ipv4, tcp
=======
    SSL_library_init();
    SSL_CTX *ctx = init_ssl();

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // ipv4, tcp
    struct sockaddr_in addr = {AF_INET, htons(4433), INADDR_ANY};
>>>>>>> 2f0e4a7 (update)
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

<<<<<<< HEAD
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
=======
    bind(server_fd, (struct(sockaddr *)&addr, sizeof(addr)));
    listen(server_fd, 5);
    
    printf("Server running on https://127.0.0.1:%d\n", port);

    while(1) {
        // accept connect from client
        client_fd = accept(server_fd, NULL, NULL);
>>>>>>> 2f0e4a7 (update)
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        handle_client(client_fd, argv[1], argv[2], argv[3]);
        close(client_fd);
    }

    close(server_fd);
}