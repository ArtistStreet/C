#include "lib.h"

uint8_t count(const char *str) {
    int cnt = 0;
    while (*str) {
        if (*str == '-') {
            cnt++;
        }
        str++;
    }
    return cnt;
}

void cd(const char *names, file_system *fs, bool print_pwd) {
    char path[100];

    if (strcmp(names, "~") == 0) { 
        fs->current = fs->root; 
        return;
    } else if (names[0] == '~' && strlen(names) > 1) {
        fs->current = fs->root;
        strcpy(path, names + 1);
    } else {
        strcpy(path, names);
    }

    uint8_t temp = count(names); // handle `cd -`
    if (temp > 0) {
        for (uint8_t i = 0; i < temp; i++) {
            if (fs->current->parent) fs->current = fs->current->parent;
        }
        if (print_pwd) pwd(fs);
        return;
    }

    node *target = find_node(fs->current, path);
    if (target && target->isDir) {
        fs->current = target;
    } else {
        printf("Directory not found: %s\n", path);
    }

    if (print_pwd) pwd(fs);
}

