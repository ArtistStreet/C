#ifndef LIB_H
#define LIB_H

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

typedef struct {
    char *file_name;
    char *content;
} FileEntry;

void handle_token(const char *name, const char *pass);
SSL_CTX *init_ssl();
void log_request(int client_fd, const char *method, const char *path);
FileEntry *load_file(const char *folder_path, int *count);
void free_files(FileEntry *files, int count);
const char *get_file_content(const char *file_name, FileEntry *files, int count);

#endif