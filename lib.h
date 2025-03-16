#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <malloc.h>

extern char choice[100], buffer[100];
extern int8_t loop, cnt_tab, cnt_enter, i, slash;     

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

typedef struct {
    char *first;
    int8_t second;
} pair;

extern pair *l;

file_system *init();

int32_t hash_for_input(char *str);

int32_t hash_for_file(char *str, int8_t len);

bool check_name(const char *name, node *current); 

void split_path(const char *path, char *parent, char *last_part);

void mkdir(const char *names, file_system *fs); 

void touch(const char *names, file_system *fs); 

void REMOVE(const char *names, file_system *fs, const int check); 

void cd(const char *names, file_system *fs, bool print_pwd);

node *get_destination(file_system *fs, const char *path, char *token, node *original_current);

char *handle_tab(file_system *fs, char *res, char *before_slash, int8_t cnt_tab, int8_t *loop, char *buffer, int8_t *current_location);

void ls_path(file_system *fs, const char *path);

void ls(file_system *fs, const char *path); 

void pwd(file_system *fs);

node *find_node(node *root, const char *name);

node *build_node(node *root, const char *name);

void save_file_system(file_system *fs, const char *filename);

void save_current_cursor(file_system *fs);

node *load_current_cursor(file_system *fs);

file_system *load_file_system(const char *filename);

void enableRawMode(struct termios *oldt);

void disableRawMode(struct termios *oldt);

void mv(const char *path, file_system *fs);

void help();

#endif // LIB_H