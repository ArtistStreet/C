#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

extern char *save_file_with_similar_name[200], save_tab[20];
extern char choice[100], buffer[100];
extern uint8_t loop, cnt_tab, i;     

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

int32_t hash_for_input(char *str);

int32_t hash_for_file(char *str, int8_t len);

bool check_name(const char *name, node *current); 

char *check_string(char *string, bool *check_string_end, char **before_slash);

void mkdir(const char *names, file_system *fs); 

void touch(const char *names, file_system *fs); 

void REMOVE(const char *names, file_system *fs, const int check); 

void cd(const char *names, file_system *fs, bool print_pwd);

char *handle_tab(file_system *fs, char *res, char *before_slash,uint8_t cnt_tab, uint8_t *loop);

void ls(file_system *fs, const char *path); 

void pwd(file_system *fs);

node *find_node(node *root, const char *name);

void save_file_system(file_system *fs, const char *filename);

void save_current_cursor(file_system *fs);

node *load_current_cursor(file_system *fs);

file_system *load_file_system(const char *filename);

void enableRawMode(struct termios *oldt);

void disableRawMode(struct termios *oldt);

void mv();

void help();

#endif // LIB_H