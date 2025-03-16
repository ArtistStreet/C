// fuck i tired with auto complete

#include "lib.h"

void rm(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 0);
}

void rm_dir(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 1);
}

void split_path(const char *path, char *parent, char *last_part) {
    // if (strcmp(parent, '\0') == 0 || strcmp(last_part, '\0') == 0) {
    //     return;
    // }
    char *last_slash = strrchr(path, '/'); 

    if (last_slash && last_slash != path) { 
        size_t index = last_slash - path; 
        strncpy(parent, path, index); 
        parent[index] = '\0';
        
        strcpy(last_part, last_slash + 1); 
    } else if (last_slash == path) {
        strcpy(parent, "/");
        strcpy(last_part, "");
    } else { 
        strcpy(parent, ""); 
        strcpy(last_part, path);
    }
}

void print_memory_usage() {
    struct mallinfo2 mi = mallinfo2();
    printf("Total allocated: %ld bytes\n", mi.uordblks);  
    printf("Total free: %ld bytes\n", mi.fordblks);       
}

int main() {
    file_system *fs = load_file_system("filesystem.txt"); 
    printf("press \033[1;31m%s\033[0m for more information\n", "help");
    pwd(fs);
    struct termios oldt;
    enableRawMode(&oldt);
    
    memset(buffer, 0x0, sizeof(buffer));
    char *string = NULL, *save_current_location = NULL; // directory before slash
    int8_t len = 0;

    while (1) {
        char c = getchar();
        char *tok = NULL;
        bool print_pwd = true;
        // TODO: handle tab
        // TODO: complete mv command
        if (c == '\t') {
            if (buffer[0] == '\0') continue;
            
            // if (cnt_tab < 1) {
                string = strrchr(buffer, ' ');
                if (string) {
                    string += 1;
                }
            // }
            // printf("%s", buffer);
            // // strcpy(string, buffer);
            // printf("123");
            // fflush(stdout);
            // printf("%s ", string); // Ensure the string is printed
            // fflush(stdout); // Ensure the output is flushed
            
            cnt_tab++;
            cnt_enter = 0;
            char parent[100], last_part[100];
            split_path(string, parent, last_part);

            save_current_location = handle_tab(fs, last_part, parent, cnt_tab, &loop, buffer, &i);
            if (save_current_location != NULL) {
                strcat(buffer, save_current_location);
                // i = strlen(save_current_location);
            }
            // printf("%d", i);
            // FIXME: delete all except ' ' and /
            for (int i = strlen(buffer); i >= 0; i--) {
                if (buffer[i] != ' ' && buffer[i] != '/') {
                    buffer[i] = '\0'; 
                } else {
                    break;
                }
            }
            
            continue;
        }
        else if (c == ' ') {
            cnt_tab = 0;   
            cnt_enter = 0;  
            string = NULL, save_current_location = NULL;
            for (int8_t j = len; j > i; j--) {
                buffer[j] = buffer[j - 1];
            }

            buffer[i++] = c;
            len++;
            printf("\r%s", buffer);
            printf("\r");
            for (int j = 0; j < i; j++) printf("\033[C");
            continue;
        } 
        else if (c == 127) { // handle backspace
            if (i > 0) {
                i--;
                len--;
                for (int j = i; j < len; j++) {
                    buffer[j] = buffer[j + 1];
                }
                buffer[len] = '\0';

                printf("\r%s ", buffer); 
                printf("\r");
                for (int j = 0; j < i; j++) printf("\033[C"); 
            }
            continue;
        } else if (c == 27) {  
            if (getchar() == '[') { 
                char arrow = getchar();
                if (arrow == 'D') { 
                    if (i > 0) {
                        i--;
                        printf("\033[D");  
                    }
                } else if (arrow == 'C') { 
                    if (i < len) {
                        i++;
                        printf("\033[C");  
                    }
                }
            }
            continue;
        } else {
            if (c == '\n') {
                if (save_current_location != NULL && cnt_enter == 0) {
                    for (int j = i - 1; j >= 0; j--) {
                        if (buffer[j] != ' ' && buffer[j] != '/') {
                            buffer[j] = '\0'; 
                        } else {
                            strcat(buffer, save_current_location);
                            break;
                        }
                    }
                }
                cnt_enter++;
                if (cnt_enter == 1 && save_current_location != NULL && cnt_tab != 1) {
                    printf("\n");
                    printf("\r\033[K");
                    printf("\r\033[A");  // move cursor up
                    printf("\r\033[K");
                    // printf("buf: %s", buffer);
                    if (l[slash].second) 
                        strcat(buffer, "/");
                    printf("%s", buffer);
                    loop = 0;
                    continue;
                } 
                else {
                    printf("\n");
                    printf("\r\033[K");
                    if (i > 0)
                        strcpy(choice, buffer);
                    tok = strtok(choice, " ");
                    if (tok == NULL) {
                        continue;
                    }
                    cnt_enter = 0, i = 0, cnt_tab = 0, loop = 0, slash = 0;
                    string = NULL, save_current_location = NULL;
                    memset(buffer, 0x0, sizeof(buffer));
                }
            } else {
                for (int8_t j = len; j > i; j--) {
                    buffer[j] = buffer[j - 1];
                }

                buffer[i++] = c;
                len++;
                printf("\r%s", buffer);
                printf("\r");
                for (int j = 0; j < i; j++) printf("\033[C");
                continue;
            }
        }

        if (strcmp(tok, "mkdir") == 0) {
            tok = strtok(NULL, " ");
            mkdir(tok, fs);
        } else if (strcmp(tok, "ls") == 0) {
            tok = strtok(NULL, " ");
            ls(fs, tok);
        } else if (strcmp(tok, "touch") == 0) {
            tok = strtok(NULL, " ");
            touch(tok, fs);
        } else if (strcmp(tok, "cd") == 0) {
            tok = strtok(NULL, " ");
            cd(tok, fs, print_pwd);
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
            tok = strtok(NULL, "\0");
            mv(tok, fs);
        } else if (strcmp(tok, "help") == 0) {
            help();
        } else {
            if (c != '\n')
                printf("%s: command not found\n", tok);
            disableRawMode(&oldt); // disable raw mode before exiting
        }
        memset(choice, 0, sizeof(choice));
    }

    disableRawMode(&oldt); // disable raw mode before exiting
    print_memory_usage();

    return 0; 
}