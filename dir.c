// fuck i tired with auto complete

#include "lib.h"

void rm(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 0);
}

void rm_dir(const char *names, file_system *fs, const int isDir) {
    REMOVE(names, fs, 1);
}

int main() {
    file_system *fs = load_file_system("filesystem.txt"); 
    printf("press \033[1;31m%s\033[0m for more information\n", "help");
    pwd(fs);
    struct termios oldt;
    enableRawMode(&oldt);
    
    // save_tab[0] = 'c', save_tab[1] = 'd', save_tab[2] = ' ';
    memset(buffer, 0x0, sizeof(buffer));
    char *string = NULL, *before_slash = NULL, *res = NULL, *save_current_location = NULL; // directory before slash

    while (1) {
        char c = getchar();
        char *tok = NULL;
        bool print_pwd = true;
        // TODO: handel tab
        // FIXME: fix auto complete when press tab 
        if (c == '\t') {
            bool check_string_end = false;
            if (cnt_tab < 1) {
                string = strchr(buffer, ' ');
                if (string) {
                    string += 1;
                }
                res = check_string(string, &check_string_end, &before_slash);
            }

            if (check_string_end == true) { // if string end with /
                printf("\n");
                ls(fs, res);
            } else {
                cnt_tab++;
                if (before_slash == NULL) {
                    before_slash = (char *)malloc(2 * sizeof(char));

                    if (before_slash == NULL) {
                        printf("Memory allocation failed\n");
                        exit(1);
                    }

                    strcpy(before_slash, "~");
                }
                // printf("%s\n", buffer);
                save_current_location = handle_tab(fs, res, before_slash, cnt_tab, &loop, buffer, &i);
                // printf("%d\n", i);
            }

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
                string = NULL, before_slash = NULL, res = NULL, save_current_location = NULL; // directory before slash
                
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