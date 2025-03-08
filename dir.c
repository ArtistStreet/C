// ahhh i tired with cd command

#include "lib.h"

int count(const char *str) {
    int cnt = 0;
    while (*str) {
        if (*str == '-') {
            cnt++;
        }
        str++;
    }
    return cnt;
}

void cd(const char *names, file_system *fs) {
    if (strcmp(names, "~") == 0) {
        fs->current = fs->root; // move to root
        return;
    }

    __uint8_t temp = count(names);
    if (temp > 0) {
        for (__uint8_t i = 0; i < temp; i++) {
            fs->current = fs->current->parent;
        }
        return; 
    }

    char path[100];
    strcpy(path, names);

    char *token = strtok(path, "/"); // tokenize the path by "/"
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
            if (found == false) {
                printf("Directory not found\n");
                return;
            }
        }
        token = strtok(NULL, "/"); // get next token
    }
}

void rm(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 0);
}

void rm_dir(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 1);
}

void mv() {

}



char *save_file_with_similar_name[200];

char *handle_tab(file_system *fs, char *sub, __int8_t cnt, __int8_t cnt_tab, __int8_t *loop) {
    node *current = fs->current->child;

    while (current != NULL) {
        if (hash_for_input(sub) == hash_for_file(current->name, strlen(sub)) && current->isDir == 1) {
            save_file_with_similar_name[cnt] = current->name;
            (cnt)++;
        }
        current = current->next;
    }

    if (cnt == 1) {
        char *result = strdup(save_file_with_similar_name[0]);
        return result;
    } else {
        if (cnt_tab == 1) {
            printf("\n");
            for (size_t i = 0; i < cnt; i++) {
                printf("%s ", save_file_with_similar_name[i]);
            }
            printf("\033[A"); // Move cursor up
            printf("\r");
            printf("cd %s", save_file_with_similar_name[0]);
        } else {
            printf("\r\033[K"); 
            printf("cd %s", save_file_with_similar_name[*loop % cnt]);
            fflush(stdout);  
            
            printf("\033[s");  // save current location
            
            printf("\n");
            
            for (size_t i = 0; i < cnt; i++) {
                if (i == *loop % cnt) {
                    printf("\033[1;32m%s\033[0m ", save_file_with_similar_name[i]); // In màu xanh thư mục được chọn
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
    return NULL;
}

int main() {
    file_system *fs = load_file_system("filesystem.txt"); 
    printf("press \033[1;31m%s\033[0m for more information\n", "help");
    pwd(fs);
    struct termios oldt;
    enableRawMode(&oldt);
    
    char choice[100], buffer[100];
    memset(buffer, 0x0, sizeof(buffer));
    int index = 0, cnt_tab = 0;
    __int8_t cnt = 0, loop = 0;

    while (1) {
        char c = getchar();

        if (index == 3 && strncmp(buffer, "cd ", 3) == 0 && c == '\t') {
            printf("\n");
            ls(fs);
            memset(buffer, 0x0, sizeof(buffer));
            index = 0;
            continue;
        } else if (index > 3 && strncmp(buffer, "cd ", 3) == 0 && c == '\t') {
            cnt_tab++;
            char *sub = buffer + 3;
            char *completion = handle_tab(fs, sub, cnt, cnt_tab, &loop);
        
            if (completion != NULL) { 
                snprintf(buffer, sizeof(buffer), "cd %s", completion); // set buffer to completion
                index = strlen(buffer);
        
                printf("\r%s", buffer);
                fflush(stdout);

                free(completion);
            } 
            else if (cnt_tab != 1) {
                strcpy(choice, buffer);
                // snprintf(buffer, sizeof(buffer), "cd %s", completion);
                // index = strlen(buffer);
        
                // printf("\r%s", buffer);
                // fflush(stdout);
    
                // free(completion);
            }
        
            continue;
        } else if (c == 127) { // handle backspace
            if (index > 0) {
                index--;
                printf("\b \b"); // move cursor back, print space, move cursor back again
            }
            continue;
        } else {
            if (c == '\n') {
                buffer[index] = '\0';
                printf("\n"); // print newline
                strcpy(choice, buffer);
                index = 0;
                cnt_tab = 0;
                cnt = 0;
                loop = 0;
            } else {
                buffer[index++] = c;
                printf("%c", c); // echo the character
                continue;
            }
        }

        char *tok = strtok(choice, " ");
        // printf("%s\n", tok);
        if (strcmp(tok, "mkdir") == 0) {
            tok = strtok(NULL, " ");
            mkdir(tok, fs);
        } else if (strcmp(tok, "ls") == 0) {
            ls(fs);
        } else if (strcmp(tok, "touch") == 0) {
            tok = strtok(NULL, " ");
            touch(tok, fs);
        } else if (strcmp(tok, "cd") == 0) {
            tok = strtok(NULL, " ");
            cd(tok, fs);
        } else if (strcmp(tok, "exit") == 0) {
            save_file_system(fs, "filesystem.txt");
            save_current_cursor(fs); // save current location before exit
            break;
        } else if (strcmp(tok, "pwd") == 0) {
            pwd(fs);
        } else if (strcmp(tok, "rm") == 0) {
            tok = strtok(NULL, " ");
            rm(tok, fs, 0);
        } else if (strcmp(tok, "rmdir") == 0) {
            tok = strtok(NULL, " ");
            rm_dir(tok, fs, 1);
        } else if (strcmp(tok, "mv") == 0) {

        } else if (strcmp(tok, "help") == 0) {
            printf("\033[1;31m%s\033[0m: create a directory\n", "mkdir");
            printf("\033[1;31m%s\033[0m: list files and directories\n", "ls");
            printf("\033[1;31m%s\033[0m: create a file\n", "touch");
            printf("\033[1;31m%s\033[0m: change directory\n", "cd");
            printf("\033[1;31m%s\033[0m: print working directory\n", "pwd");
            printf("\033[1;31m%s\033[0m: remove a file\n", "rm");
            printf("\033[1;31m%s\033[0m: remove a directory\n", "rmdir");
            printf("\033[1;31m%s\033[0m: move (remane) file or directory\n", "mv");
            printf("\033[1;31m%s\033[0m: exit the program\n", "exit");
        }
        else {
            printf("%s: command not found\n", tok);
            disableRawMode(&oldt); // disable raw mode before exiting
            return 1;
        }
        memset(buffer, 0x0, sizeof(buffer));
    }

    disableRawMode(&oldt); // disable raw mode before exiting
    return 0; 
}