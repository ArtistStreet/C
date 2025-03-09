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
        return; 
    }

    token = strtok(path, "/"); // tokenize the path by "/"
    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            if (fs->current->parent != NULL) {
                fs->current = fs->current->parent; // move to parent
            }
        } else {
            node *temp = fs->current->child; // get child of current node
            bool found = false;
            while (temp != NULL) {
                if (strcmp(token, temp->name) == 0) {
                    if (temp->isDir) {
                        fs->current = temp; // move to the directory;
                        found = true;
                        break;
                    } else {
                        printf("Not a directory\n");
                        return;
                    }
                }
                temp = temp->next; // get next node
            }
            if (found == false && print_pwd == true) {
                printf("Directory not found\n");
                return;
            }
        }
        token = strtok(NULL, "/"); // get next token
    }
    if (print_pwd) pwd(fs);
}

char *handle_tab(file_system *fs, char *sub, uint8_t cnt_tab, uint8_t *loop) {
    node *current = fs->current->child;
    char *result = NULL;
    uint8_t cnt = 0;

    while (current != NULL) {
        if (hash_for_input(sub) == hash_for_file(current->name, strlen(sub)) && current->isDir == 1) {
            save_file_with_similar_name[cnt] = current->name;
            (cnt)++;
        }
        current = current->next;
    }

    if (cnt == 1) {
        result = strdup(save_file_with_similar_name[0]);
        return result;
    } else if (cnt > 1) {
        if (cnt_tab == 1) {
            printf("\n");
            for (size_t i = 0; i < cnt; i++) {
                printf("%s ", save_file_with_similar_name[i]);
            }
            printf("\033[A"); // Move cursor up
            printf("\r");
            printf("cd %s", save_file_with_similar_name[0]);
        } else if (cnt_tab > 1) {
            printf("\r\033[K"); 
            printf("cd %s", save_file_with_similar_name[*loop % cnt]);
            fflush(stdout);  
            
            printf("\033[s");  // save current location        
            printf("\n");

            for (size_t i = 0; i < cnt; i++) {
                if (i == *loop % cnt) {
                    printf("\033[1;32m%s\033[0m ", save_file_with_similar_name[i]); // In màu xanh thư mục được chọn
                    memset(save_tab + 3, 0x0, sizeof(save_tab) - 3);
                    strcat(save_tab, save_file_with_similar_name[i]);
                } else {
                    printf("%s ", save_file_with_similar_name[i]);
                }
            }
            
            printf("\033[K");
            printf("\033[u"); // restore cursor
            fflush(stdout);
            
            (*loop)++;
        }
    }
    return result ? result : NULL;
}