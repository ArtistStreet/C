#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

typedef struct node {
    char name[100]; // fix variable name
    int isDir; // 1 is dir, 0 is file
    struct node *parent;
    struct node *child;
    struct node *next; // manage file or dir in the same dir
} node;

typedef struct file_system {
    node *root; // root node
    node *current; // current node
} file_system;

file_system *init();

__int32_t get_hash(char *str, __int8_t len);

__int32_t hash_for_input(char *str);

__int32_t hash_for_file(char *str, __int8_t len);

bool check_name(const char *name, node *current); 

void mkdir(const char *names, file_system *fs); 

void touch(const char *names, file_system *fs); 

void REMOVE(const char *names, file_system *fs, const int check); 

void ls(file_system *fs); 

void pwd(file_system *fs);

void save_node(FILE *file, node *n);

void save_file_system(file_system *fs, const char *filename);

node *find_node(node *root, const char *name);

node *load_node(FILE *file, node *root);

void save_current_cursor(file_system *fs);

node *load_current_cursor(file_system *fs);

file_system *load_file_system(const char *filename);

void enableRawMode(struct termios *oldt);

void disableRawMode(struct termios *oldt);

#endif // LIB_H