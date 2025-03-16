#include "lib.h"

void ls_path(file_system *fs, const char *path) {
    char *token = NULL;
    node *original_current = fs->current; // save the original current node

    node *current = get_destination(fs, path, token, original_current);  // list the contents of the directory
    bool check = true;
    while (current != NULL) {
        check = false;
        if (current->isDir) {
            printf("\033[1;32m%s\033[0m ", current->name); 
        } else {
            printf("\033[1;35m%s\033[0m ", current->name);
        }
        current = current->next;
    }
    if (!check)
        printf("\n");

    fs->current = original_current; // restore the original current node
}

void ls(file_system *fs, const char *path) {
    if (path == NULL || strcmp(path, "") == 0) { // current directory
        ls_path(fs, fs->current->name);  
    } else {
        ls_path(fs, path);
    }
}