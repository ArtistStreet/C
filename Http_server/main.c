#include "lib.h"

#define port 4433
#define buffer_size 4096

typedef struct 
{
    const char *path;
    void (*handler)(SSL *ssl);
} Route;

// server reads index file and send content to client using socket
void send_file_content(SSL *ssl, const char *file_name, const char *content_type) {
    FILE *file = fopen(file_name, "rb");

    FILE *server_log = fopen("log/server.log", "a");
    // printf("Debug: %s\n", server_log);
    // printf("")
    if (!server_log) {
        printf("Error\n");
    }
    if (file) {
        char header[256];
        char buffer[1024];
        size_t bytes_read;

        snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);

        SSL_write(ssl, header, strlen(header));

        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            SSL_write(ssl, buffer, (bytes_read));
        }

        fclose(file);
    } else {
        // printf("Error: Failed to open file %s\n", file_name);
        char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        SSL_write(ssl, not_found, strlen(not_found));
        fprintf(server_log, "Error: Failed to open file %s\n", file_name);
        fwrite(not_found, 1, strlen(not_found), server_log);
    }

    if (server_log) {
        fclose(server_log);
    }
}

void handle_home(SSL *ssl) {
    send_file_content(ssl, "Web/index.html", "text/html");
}

void handle_about(SSL *ssl) {
    send_file_content(ssl, "Web/about.html", "text/html");
}

Route route[] = {
    {"/", handle_home},
    {"/about", handle_about}
};

void serve_static_file(SSL *ssl, const char *url) {
    // printf("Debug: %s\n", url);
    char file_path[256];
    
    char *content_type = "text/plain";
    if (strcmp(url, "/") == 0) {
        snprintf(file_path, sizeof(file_path), "Web/index.html");
        content_type = "text/html"; // Ensure correct content type for HTML
    } else {
        snprintf(file_path, sizeof(file_path), "Web%s", url);
    }
    
    if (strstr(url, ".html")) {
        content_type = "text/html"; // Ensure correct content type for HTML
    } else if (strstr(url, ".css")) {
        content_type = "text/css";
    } else if (strstr(url, ".js")) {
        content_type = "application/javascript";
    } else if (strstr(url, ".jpg")) {
        content_type = "image/jpeg";
    } else if (strstr(url, ".png")) {
        content_type = "image/png";
    }
    fflush(stdout);
    // printf("Debug: %s\n", content_type);
    send_file_content(ssl, file_path, content_type);
}

void handle_client(u_int32_t client_fd, SSL_CTX *ctx, const char *html, const char *css, const char *js) {
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
    printf("debug: %s\n", buffer);
    if (strncmp(buffer, "GET ", 4) == 0) {
        
        char *start = buffer + 4; // start from 5th char
        char *end = strchr(start, ' ');
        *end = '\0';
        // printf("debug: %s\n", start);
        log_request(client_fd, "GET", start);
        serve_static_file(ssl, start);
    } 
    else if (strncmp(buffer, "POST ", 5) == 0) {
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

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <html_file> <css_file> <js_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int opt = 1;

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
        handle_client(client_fd, ctx, argv[1], argv[2], argv[3]);
    }

    SSL_CTX_free(ctx);
    close(server_fd);
}