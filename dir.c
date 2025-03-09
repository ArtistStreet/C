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
    
    save_tab[0] = 'c', save_tab[1] = 'd', save_tab[2] = ' ';
    memset(buffer, 0x0, sizeof(buffer));

    while (1) {
        char c = getchar();
        char *tok;
        bool print_pwd = true;

        // printf("%c\n", buffer[i]);
        if (i >= 3 && (strncmp(buffer, "cd ", 3) == 0 || strncmp(buffer, "mv ", 3) == 0) && c == '\t' && buffer[i - 1] == '/') {
            printf("\n");
            strcpy(choice, buffer);
            tok = strtok(choice, " ");
            tok = strtok(NULL, " ");
            print_pwd = false;
            // printf("%s\n", buffer);
            // printf("%s\n", choice);
            cd(tok, fs, print_pwd);
            ls(fs, tok);
            printf("\033[A");
            // cnt_tab++;
            // char *sub = buffer + 3;
            // char *completion = handle_tab(fs, sub, cnt_tab, &loop);
            // printf("%s\n", completion);
            memset(buffer, 0x0, sizeof(buffer));
            i = 0;
            continue;
        } else if (i > 3 && (strncmp(buffer, "cd ", 3) == 0 || strncmp(buffer, "mv ", 3) == 0) && c == '\t') {
            cnt_tab++;
            char *sub = buffer + 3;
            char *completion = handle_tab(fs, sub, cnt_tab, &loop);
        
            if (completion != NULL) { 
                snprintf(buffer, sizeof(buffer), "cd %s", completion); // set buffer to completion
                i = strlen(buffer);
        
                printf("\r%s", buffer);
                fflush(stdout);

                free(completion);
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
                buffer[i] = '\0';
                printf("\n"); // print newline
                if (i > 0)
                    strcpy(choice, buffer);
                i = 0;
                cnt_tab = 0;
                loop = 0;
            } else {
                buffer[i++] = c;
                printf("%c", c); // echo the character
                continue;
            }
        }
        
        if (cnt_tab == 0) {
            printf("\r\033[K");
        }

        if (strlen(save_tab) > 3) {
            tok = strtok(save_tab, " ");
        } else {
            tok = strtok(choice, " ");
        }

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