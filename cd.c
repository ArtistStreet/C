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
    // 1 print, 0 no
    char *token = NULL;
    char path[100], first_char = names[0];
    
    if (strcmp(names, "~") == 0 && strlen(names) == 1) {
        fs->current = fs->root; // move to root
        return;
    } else if (first_char == '~' && strlen(names) > 2) {
        fs->current = fs->root;
        // token = strtok(path, "/"); // tokenize the path by "/"
        strcpy(path, names + 2);
    } else {
        strcpy(path, names);
    }
    
    uint8_t temp = count(names); // handle cd -
    if (temp > 0) {
        for (uint8_t i = 0; i < temp; i++) {
            fs->current = fs->current->parent;
        }
        pwd(fs);
        return; 
    }

    token = strtok(path, "/"); // tokenize the path by "/"
    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            if (fs->current->parent != NULL) {
                fs->current = fs->current->parent; // move to parent
            }
        } else {
            node *target = find_node(fs->current, token);
            // printf("%s %s\n", fs->current->name, token);
            if (target != NULL) {
                if (target->isDir) {
                    fs->current = target; 
                } else {
                    printf("Not a directory\n");
                }
            } else {
                printf("Directory not found\n");
            }
        }
        token = strtok(NULL, "/"); // get next token
    }
    // printf("1");
    if (print_pwd) pwd(fs);
}

