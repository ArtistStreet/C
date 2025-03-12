// ahhh i tired with cd command

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
        char *tok;
        bool print_pwd = true;
        // TODO: handel tab
        // FIXME: fix auto complete when press tab 
            // if (cnt_tab == 1) {
            //     for (int j = 0; j < i; j++) {
            //         printf("%c ", buffer[i]);
            //     }
            //     return 0;
            // } 
        if (c == '\t') {
            // strcpy(choice, buffer + 3);
            // first tab print list, second select items
            if (cnt_tab < 1) {
                string = strchr(buffer, ' ');
                if (string) {
                    string += 1;
                }
            }

            bool check_string_end = false;
            if (cnt_tab < 1) {
                res = check_string(string, &check_string_end, &before_slash);
            }
            // printf("%ld\n", strlen(string));
            // printf("%s %s\n", before_slash, res);

            if (check_string_end == true) { // if string end with /
                printf("12");
                ls(fs, res);
            } else {
                // printf("%s\n", before_slash);
                cnt_tab++;
                // printf("%d ", cnt_tab);
                // printf("%s\n", save_current_location);
                // ls(fs, before_slash);
                // for (size_t i = 0; i < 10; i++) {
                //     printf("%s ", save_file_with_similar_name[i]);
                // }
                handle_tab(fs, res, before_slash, cnt_tab, &loop);
            }
                // printf("%s\n", save_current_location);
            // for (size_t j = 0; j < i; j++) {
            //     printf("%c", buffer[j]);
            // }
            // printf("\n%d\n", i);
            // memset(buffer, 0x0, sizeof(buffer));
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
                // printf("%s\n", save_current_location);
                // if (save_current_location != NULL) {
                //     cd(save_current_location, fs, 1);
                // }
                // if (cnt_tab > 1) {
                //     char *dir = 
                // }
                // buffer[i] = '\0';
                printf("\n");
                if (i > 0)
                    strcpy(choice, buffer);
                i = 0;
                cnt_tab = 0;
                loop = 0;
                string = NULL, before_slash = NULL, res = NULL; // directory before slash
            } else {
                buffer[i++] = c;
                printf("%c", c); // echo the character
                continue;
            }
        }
        
        // if (cnt_tab == 0) {
        //     printf("\r\033[K");
        // }

        // if (strlen(save_tab) > 3) {
        //     tok = strtok(save_tab, " ");
        // } else {
            tok = strtok(choice, " ");
        // }

        if (strcmp(tok, "mkdir") == 0) {
            tok = strtok(NULL, " ");
            mkdir(tok, fs);
        } else if (strcmp(tok, "ls") == 0) {
            tok = strtok(NULL, " ");
            // printf("%s\n", tok);
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
        memset(buffer, 0x0, sizeof(buffer));
    }

    disableRawMode(&oldt); // disable raw mode before exiting
    return 0; 
}