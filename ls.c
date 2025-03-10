#include "lib.h"

void ls_path(file_system *fs, const char *path) {
    char name[100];
    strcpy(name, path);
    node *target = find_node(fs->root, (strcmp(fs->root->name, path) == 0) ? path : name);

    if (!target || !target->isDir) {
        printf("ls: cannot access '%s': No such directory\n", path);
        return;
    }

    node *current = target->child; 
    int i = 0;

    while (current != NULL) {
        if (current->isDir) {
            printf("\033[1;32m%s\033[0m ", current->name);
        } else {
            printf("\033[1;35m%s\033[0m ", current->name);
        }
        save_file_with_similar_name[i++] = current->name;
        current = current->next;
    }

    printf("\n");
}

void ls(file_system *fs, const char *path) {
    if (path == NULL || strcmp(path, "") == 0) { // current directory
        ls_path(fs, fs->current->name);  
    } else {
        if (path[0] == '~') { 
            char new_path[100];
            snprintf(new_path, sizeof(new_path), "%s%s", fs->root->name, path + 1);
            // printf("%s\n", new_path);
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