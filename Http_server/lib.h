#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <mongoc/mongoc.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <dirent.h>
#include <sys/stat.h>

void handle_token(const char *name, const char *pass);
SSL_CTX *init_ssl();
void log_request(int client_fd, const char *method, const char *path);
