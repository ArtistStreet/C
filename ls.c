#include "lib.h"

void ls_path(file_system *fs, const char *path) {
    char *token = NULL;
    char name[100];
    node *original_current = fs->current; // save the original current node

    if (path[0] == '~') {
        snprintf(name, sizeof(name), "%s%s", fs->root->name, path + 1);
    } else {
        strcpy(name, path);
    }

    token = strtok(name, "/"); // tokenize the path by "/"

    if (strcmp(token, "~") == 0) {
        fs->current = fs->root;
        token = strtok(NULL, "/");
    }

    while (token != NULL) {
        node *target = find_node(fs->current, token);
        if (target != NULL) {
            if (target->isDir) {
                fs->current = target; 
            } else {
                printf("Not a directory\n");
                fs->current = original_current; // restore the original current node
                return;
            }
        } else {
            printf("Directory not found\n");
            fs->current = original_current; // restore the original current node
            return;
        }
        token = strtok(NULL, "/"); // get next token
    }

    node *current = fs->current->child; // list the contents of the directory
    while (current != NULL) {
        if (current->isDir) {
            printf("\033[1;32m%s\033[0m ", current->name);
        } else {
            printf("\033[1;35m%s\033[0m ", current->name);
        }
        current = current->next;
    }
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