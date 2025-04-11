#include "lib.h"

#define port 4433
#define buffer_size 4096

typedef struct 
{
    const char *path;
    void (*handler)(SSL *ssl);
} Route;

void send_loaded_content(SSL *ssl, const char *content, size_t size, const char *content_type) {
    char header[256];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
    SSL_write(ssl, header, strlen(header));
    SSL_write(ssl, content, (size));
}

// Route route[] = {
//     {"/", handle_home},
//     {"/about", handle_about}
// };

// void process_route() {

// }

void handle_client(u_int32_t client_fd, SSL_CTX *ctx, FileEntry *files, int count) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(client_fd);
        return;
    }
    
    FILE *server_log = fopen("log/server.log", "a");
    
    char buffer[buffer_size];
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        SSL_free(ssl);
        close(client_fd);
        perror("Read Failed");
        return;
    }

    buffer[bytes_read] = '\0';
    
    if (strncmp(buffer, "GET ", 4) == 0) {
        char *start = buffer + 4; // start from 5th char
        char *end = strchr(start, ' ');
        *end = '\0';
        // printf("debug: %s\n", start);
        log_request(client_fd, "GET", start);

        char file_path[256];
        // printf("debug: %s", file_path);
    
        char *content_type = "text/plain";
        if (strcmp(start, "/") == 0) {
            strcpy(file_path, "index.html");
            content_type = "text/html";
        } else {
            snprintf(file_path, sizeof(file_path), "%s", start + 1); // skip the '/'
        }
        
        if (strstr(start, ".html")) {
            content_type = "text/html"; // Ensure correct content type for HTML
        } else if (strstr(start, ".css")) {
            content_type = "text/css";
        } else if (strstr(start, ".js")) {
            content_type = "application/javascript";
        } else if (strstr(start, ".jpg")) {
            content_type = "image/jpeg";
        } else if (strstr(start, ".png")) {
            content_type = "image/png";
        }

        const char *file_data = get_file_content(file_path, files, count);
        size_t size = get_file_size(file_path, files, count);
        if (file_data) {
            send_loaded_content(ssl, file_data, size, content_type);
        } else {
            char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
            SSL_write(ssl, not_found, strlen(not_found));
            if (server_log) {
                fprintf(server_log, "404 Not Found: %s\n", file_path);
            }
        }
    } 
    else if (strncmp(buffer, "POST ", 5) == 0) {
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
        SSL_write(ssl, response, strlen(response)); // Fix: send full response
    }

    if (server_log) {
        fclose(server_log);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
}

int main(int argc, char **argv) {
    int opt = 1;
    int file_count = 0;
    FileEntry *files = load_file(argv[1], &file_count);
    // printf("%d\n", file_count);

    SSL_library_init(); // init ssl library
    SSL_CTX *ctx = init_ssl(); // init 

    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // ipv4, tcp
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // reuse address
    struct sockaddr_in addr = {AF_INET, htons(port), INADDR_ANY};
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)); // bind socket
    listen(server_fd, 5);
    
    printf("Server running on https://127.0.0.1:%d\n", port);

    while(1) {
        // accept connect from client
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        handle_client(client_fd, ctx, files, file_count);
    }

    SSL_CTX_free(ctx);
    close(server_fd);
}