#include "lib.h"

void ls_path(file_system *fs, const char *path) {
    char name[100];
    strcpy(name, path);
    char *token = NULL;
    node *original_current = fs->current; // save the original current node

    token = strtok(name, "/"); // tokenize the path by "/"
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
        if (path[0] == '~') { 
            char new_path[100];
            snprintf(new_path, sizeof(new_path), "%s%s", fs->root->name, path + 1);
            if (strcmp(new_path, "~/") == 0) {  
                ls_path(fs, fs->root->name);
            } else {
                ls_path(fs, new_path);
            }
        } else {
            ls_path(fs, path);
        }
    }
}