#include "lib.h"

void rename_and_move(file_system *fs, const char *path, const char *last_path) {
    char *token = NULL;
    node *original_current = fs->current; // save the original current node
    
    node *current = get_destination(fs, path, token, original_current);
    while (current != NULL) {
        if (strcmp(current->name, last_path) == 0) {
            strcpy(current->name, last_path);
            return;
        }
        current = current->next;
    }
}

void mv(const char *path, file_system *fs) {
    bool rename = false;
    char name[100];
    strcpy(name, path);
    // for (size_t i = 0; i < strlen(path); i++) {
    //     if (path[i] == ' ') {
    //         rename = true;
    //         break;
    //     }
    // }
    char *tok = strtok(name, " ");
    char *last_tok = strrchr(name, ' ');
    printf("%s\n", last_tok);
    while (tok != NULL) {
        printf("%s ", tok);

        tok = strtok(NULL, " ");
    }
    // if (!rename) { // rename
    //     char parent[100], last_part[100];
    //     split_path(path, parent, last_part);
    //     rename_and_move(fs, path, last_part);
    // }
    // else { // move

    // }
}