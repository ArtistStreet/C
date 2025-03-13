// fuck i tired with auto complete

#include "lib.h"

void rm(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 0);
}

void rm_dir(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 1);
}

void split_path(const char *path, char *parent, char *last_part) {
    char *last_slash = strrchr(path, '/'); // Tìm dấu '/' cuối cùng
    if (last_slash) {
        size_t index = last_slash - path + 1; // Lấy vị trí của dấu '/'
        strncpy(parent, path, index); // Copy phần trước dấu '/'
        parent[index] = '\0'; // Kết thúc chuỗi

        if (*(last_slash + 1) != '\0') {
            strcpy(last_part, last_slash + 1); // Copy phần sau '/'
        } else {
            last_part[0] = '\0'; // Nếu '/' ở cuối, last_part rỗng
        }
    } else {
        // Nếu không tìm thấy '/', coi toàn bộ chuỗi là last_part
        strcpy(parent, ""); 
        strcpy(last_part, path);
    }
}

int main() {
    file_system *fs = load_file_system("filesystem.txt"); 
    printf("press \033[1;31m%s\033[0m for more information\n", "help");
    pwd(fs);
    struct termios oldt;
    enableRawMode(&oldt);
    
    // save_tab[0] = 'c', save_tab[1] = 'd', save_tab[2] = ' ';
    memset(buffer, 0x0, sizeof(buffer));
    char *string = NULL, *res = NULL, *save_current_location = NULL; // directory before slash

    while (1) {
        char c = getchar();
        char *tok = NULL;
        bool print_pwd = true;
        // TODO: handel tab
        // FIXME: fix auto complete when press tab 
        if (c == '\t') {
            // bool check_string_end = false;
            if (cnt_tab < 1) {
                string = strchr(buffer, ' ');
                if (string) {
                    string += 1;
                }
                // res = check_string(string);
            }

            cnt_tab++;
            char parent[100], last_part[100];
            split_path(string, parent, last_part);
            // printf("%s\n", buffer);
            save_current_location = handle_tab(fs, last_part, parent, cnt_tab, &loop, buffer, &i);

            if (save_current_location != NULL) {
                strcat(buffer, save_current_location);
            }

            // printf("\n");
            // printf("\r\033[K");  
            // printf("%s", buffer);
            
            continue;
        } else if (c == 127) { // handle backspace
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                printf("\b \b"); // move cursor back, print space, move cursor back again
            }
            continue;
        } else {
            if (c == '\n') {
        // printf("123");

                if (save_current_location != NULL) {
                    // printf("123");
                    for (int j = i - 1; j >= 0; j--) {
                        if (buffer[j] != ' ' && buffer[j] != '/') {
                            buffer[j] = '\0'; 
                        } else {
                            strcat(buffer, save_current_location);
                            break;
                        }
                    }
                }
                printf("\n");
                if (i > 0)
                    strcpy(choice, buffer);
                tok = strtok(choice, " ");
                if (tok == NULL) {
                    continue;
                }
                // printf("%s\n", tok);
                // printf("123");
                i = 0;
                cnt_tab = 0;
                loop = 0;
                string = NULL, res = NULL, save_current_location = NULL; // directory before slash
                
                memset(buffer, 0x0, sizeof(buffer));
            } else {
                buffer[i++] = c;
                printf("%c", c); // echo the character
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
    return 0; 
}